#include <d3dcompiler.h>
#include "GraphicsEngine.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <iostream>

#include "Includes/MeehanTimer.h"

#define STB_IMAGE_IMPLEMENTATION

#include "Engine.h"
#include "FrameBufferData.h"
#include "LightBufferData.hpp"
#include "ObjectBufferData.h"
#include "Plane.h"
#include "StringConversion.hpp"
#include "Terrain.h"
#include "Includes/External/stb_image.h"

#define REPORT_DX_WARNINGS

#pragma comment(lib, "d3dcompiler.lib")

bool GraphicsEngine::Init(int aHeight, int aWidth, HWND aWindowHandle)
{
	if (aWindowHandle == nullptr || aHeight <= 0 || aWidth <= 0)
	{
		std::cout << "Invalid window handle or window size" << std::endl;
		return false;
	}
	myWindowHandle = &aWindowHandle;
	myWindowSize = CommonUtilities::Vector2<unsigned int>(aHeight, aWidth);
	myTextureManager = std::make_unique<TextureManager>();

    std::wstring solutionDir = StringToWString(SOLUTION_DIR);
    std::wstring shaderFolder = L"TGP\\Shaders\\";
    if (!CompileShaders(solutionDir + shaderFolder))
    {
        return false;
    }
	if (!CreateDeviceAndSwapChain())
	{
		//std::cout << "Failed to create device and swap chain" << std::endl;
		return false;
	}

	if (!myTextureManager->Init(GetDevice(), GetContext()) ||
		!CreateRenderTarget(myHResult) ||
		!CreateReflectionRenderTarget(myHResult) ||
		!CreateDepthStencilState(myHResult) ||
		!CreateDepthBuffer(myHResult) ||
		!CreateLightBuffer(myHResult) ||
		!CreateFrameAndObjectBuffer(myHResult) ||
		!CreateTextureSamplerState(myHResult) ||
		!CreateRasterizerState(myHResult) ||
		!CreateBlendState(myHResult))
	{
		PrintDebugMessages();
		return false;
	}

	SetupViewport();
	CreateCamera(static_cast<float>(aWidth), static_cast<float>(aHeight));
	myCamera->SetPosition({ 0.0f, 100.0f, -100.0f });
	mySavedCamera = std::make_unique<Camera>(*myCamera);
	myReflectionPlaneHeight = 50.0f;

	myTerrain = std::make_shared<Terrain>();

    myObjectsToRender.push_back(myTerrain);
    myObjectsToRender.push_back(std::make_shared<Plane>());
    //myObjectsToRender.push_back(std::make_shared<Sphere>());

	for (auto& object : myObjectsToRender)
	{
		if (!object->Initialize(myDevice.Get()))
			return false;
	}

	return true;
}
void GraphicsEngine::Update(float aDeltaTime, CommonUtilities::InputHandler& anInputHandler)
{
	//myObjectsToRender[0]->SetPosition({5.0f + (0.001f * time) * 2.0f,0,10.0f});
	myObjectsToRender[0]->SetPosition({ 0.0f, -25.0f,100.0f });
	myObjectsToRender[1]->SetPosition({ 100.0f, myReflectionPlaneHeight,0.0f });

	UpdateTimeOfDay();

	static float time = 0.0f;
	time += aDeltaTime;

	myRenderMode = 0; // Normal mode

	if (anInputHandler.GetKeyHeld(0x31))
	{
		myRenderMode = 1;
	}
	else if (anInputHandler.GetKeyHeld(0x32))
	{
		myRenderMode = -1;
	}

	if (myCamera == nullptr)
	{
		return;
	}
	myCamera->Update(aDeltaTime, anInputHandler);

	UpdateLightBuffer();
	UpdateFrameBuffer(aDeltaTime);
}
void GraphicsEngine::UpdateFrameBuffer(float aDeltaTime = 0.0f)
{
	if (aDeltaTime != 0.0f)
		myElapsedTime += aDeltaTime;

	FrameBufferData frameBufferData = {};
	frameBufferData.worldToCamera = myCamera->GetView();
	frameBufferData.cameraToProjection = myCamera->GetProjection();
	frameBufferData.worldToClip = myCamera->GetView() * myCamera->GetProjection();
	frameBufferData.worldToClipReflected = mySavedCamera->GetView() * mySavedCamera->GetProjection();
	frameBufferData.time = myElapsedTime;
	frameBufferData.cameraPosition = myCamera->GetPosition();
	frameBufferData.waterHeight = myReflectionPlaneHeight;
	frameBufferData.resolution = CommonUtilities::Vector2<float>(static_cast<float>(myBackBufferTextureWidth), static_cast<float>(myBackBufferTextureHeight));

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	myContext->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &frameBufferData, sizeof(FrameBufferData));
	myContext->Unmap(myFrameBuffer.Get(), 0);

	myContext->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
}
void GraphicsEngine::UpdateFrameBufferForReflection()
{
	FrameBufferData frameBufferData = {};
	frameBufferData.worldToCamera = mySavedCamera->GetView();
	frameBufferData.cameraToProjection = mySavedCamera->GetProjection();
	frameBufferData.worldToClip = mySavedCamera->GetView() * mySavedCamera->GetProjection();
	frameBufferData.worldToClipReflected = mySavedCamera->GetView() * mySavedCamera->GetProjection();
	frameBufferData.time = myElapsedTime;
	frameBufferData.cameraPosition = mySavedCamera->GetPosition();
	frameBufferData.waterHeight = myReflectionPlaneHeight;
	frameBufferData.resolution = CommonUtilities::Vector2<float>(static_cast<float>(myWindowSize.x), static_cast<float>(myWindowSize.y));

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	myContext->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &frameBufferData, sizeof(FrameBufferData));
	myContext->Unmap(myFrameBuffer.Get(), 0);

	myContext->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
}
void GraphicsEngine::UpdateObjectBuffer(const std::shared_ptr<Object3D>& anObject) const
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	myContext->Map(myObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	ObjectBufferData* objectData = reinterpret_cast<ObjectBufferData*>(mappedResource.pData);
	objectData->modelToWorldMatrix = anObject->GetWorldMatrix();

	myContext->Unmap(myObjectBuffer.Get(), 0);

	myContext->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
}
void GraphicsEngine::UpdateLightBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	myContext->Map(myLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	LightBuffer* lightData = (LightBuffer*)mappedResource.pData;

	double timeOfDay = GetTimeOfDay();
	float dirLightIntensity = 0.0f;
	float ambientIntensity1 = 0.0f;
	float ambientIntensity2 = 0.0f;

	lightData->directionalLightDirection = CommonUtilities::Vector3(-0.5f, 1.0f, -0.5f);
	lightData->directionalLightColor = CommonUtilities::Vector4(1.0f, 1.0f, 0.8f, 1.0f); // Light yellow
	lightData->ambientColor1 = CommonUtilities::Vector4(1.0f, 0.0f, 0.0f, 1.0f); // Red
	lightData->ambientColor2 = CommonUtilities::Vector4(0.0f, 0.0f, 1.0f, 1.0f); // Blue

	if (timeOfDay < 6.0)
	{
		// Increase intensity from 0.0f - 1.0f hours 0 to 6
		dirLightIntensity = static_cast<float>(timeOfDay / 6.0);
		// Increase intensity from 0.0f - 1.0f hours 0 to 6
		ambientIntensity1 = static_cast<float>(timeOfDay / 6.0);
		// Decrease intensity from 1.0f - 0.0f hours 0 to 6
		ambientIntensity2 = static_cast<float>((6.0 - timeOfDay) / 6.0);
	}
	else if (timeOfDay > 18.0)
	{
		// Decrease intensity from 1.0f - 0.0f hours 18 to 24
		dirLightIntensity = static_cast<float>((24.0 - timeOfDay) / 6.0);

		ambientIntensity1 = 0.5f;
		lightData->ambientColor1 = CommonUtilities::Vector4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		// Increase intensity from 0.5f - 1.0f hours 18 to 24
		ambientIntensity2 = static_cast<float>(0.5 + ((timeOfDay - 18.0) / 6.0) * 0.5f);
		lightData->ambientColor2 = CommonUtilities::Vector4(0.0f, 0.0f, 1.0f, 1.0f); // Blue
        //ambientIntensity2 = 0.1f + static_cast<float>((timeOfDay - 18.0) / 6.0) * 0.9f; // Increase intensity from 0.1f to 1.0f hours 18 to 24
    }
	else if (timeOfDay >= 6.0 && timeOfDay <= 18.0)
	{
		dirLightIntensity = 1.0f;

		ambientIntensity1 = 0.5f;

		ambientIntensity2 = 1.0f;
		lightData->ambientColor2 = CommonUtilities::Vector4(1.0f, 0.8f, 0.8f, 1.0f); // White
	}
	lightData->directionalLightIntensity = dirLightIntensity;
	lightData->ambientIntensity1 = ambientIntensity1;
	lightData->ambientIntensity2 = ambientIntensity2;
	lightData->renderMode = myRenderMode;

	myContext->Unmap(myLightBuffer.Get(), 0);
}
void GraphicsEngine::UpdateTimeOfDay()
{
    double totalTime = Engine::GetInstance().GetTimer().GetTotalTime();
    double dayCycleTime = fmod(totalTime, 60.0); // 60 seconds for a full day cycle
    myTimeOfDay = (dayCycleTime / 60.0) * 24.0; // Convert to 24-hour format
    std::string debugMessage = "Time of day: " + std::to_string(myTimeOfDay);
    OutputDebugString(debugMessage);
}
void GraphicsEngine::Render(float aDeltaTime)
{
	myContext->OMSetRenderTargets(1, myReflectionRTV[myCurrentReflectionBufferIndex].GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myReflectionRTV[myCurrentReflectionBufferIndex].Get(), myClearColor);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	myContext->RSSetState(myRasterizerStateFrontFaceCulling.Get());

	SetBlendState();
	RenderScene(aDeltaTime, true);

	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());
	myContext->ClearRenderTargetView(myBackBuffer.Get(), myClearColor);
	myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	myContext->RSSetState(nullptr);

	SetBlendState();
	RenderScene(aDeltaTime, false);

	// Present the back buffer to the screen
	mySwapChain->Present(1, 0);
}
void GraphicsEngine::RenderScene(float aDeltaTime, bool isReflection)
{
	if (isReflection)
	{
		mySavedCamera = std::make_unique<Camera>(*myCamera);

		// Update reflected camera
		auto reflectionTransform = myCamera->GetTransform().Reflect(myReflectionPlaneHeight);
		myCamera->SetTransform(reflectionTransform);
		myCamera->CalculateView();
		UpdateFrameBuffer();

		// Turn on front face culling for reflection rendering
		myContext->RSSetState(myRasterizerStateFrontFaceCulling.Get());

		RenderObjects(true);
	}
	else
	{
		// Bind textures for the main scene, including the reflection texture
		BindTextures(myReflectionSRV[myCurrentReflectionBufferIndex].Get());

		// Use the main camera
		myCamera = std::make_unique<Camera>(*mySavedCamera);
		myCamera->CalculateView();
		UpdateFrameBuffer(aDeltaTime);

		// Turn on back face culling for main scene rendering
		myContext->RSSetState(myRasterizerState.Get());

		RenderObjects(false);
	}
}
void GraphicsEngine::RenderObjects(bool isReflection)
{
	int textureSlot = 0;

	if (isReflection)
	{
		PrintTransform(myCamera->GetTransform());
		PrintTransform(mySavedCamera->GetTransform());
	}

	for (auto& object : myObjectsToRender)
	{
		//ID3D11ShaderResourceView* srv = Engine::GetInstance().GetGraphicsEngine().GetTextureManager().GetTexture("Pyramid");

		if (object->GetPixelShaderPath().find("Plane_PS.cso") != std::string::npos && isReflection)
		{
			continue;
		}

		UpdateObjectBuffer(object);
		if(myRenderMode == 0)
		{
			object->Render(myContext.Get());
		}
		else if (myRenderMode == 1)
		{
			if (object->GetVertexShaderPath().find("Terrain_VS.cso") != std::string::npos)
			{
				myTerrain->RenderDiffuse(myContext.Get());
			}
		}
		else if (myRenderMode == -1)
		{
			if (object->GetVertexShaderPath().find("Terrain_VS.cso") != std::string::npos)
			{
				myTerrain->RenderSpecular(myContext.Get());
			}
		}
		textureSlot++;
	}
}
void GraphicsEngine::BindTextures(ID3D11ShaderResourceView* reflectionSRV)
{
	myContext->PSSetSamplers(0, 1, mySamplerState.GetAddressOf());

	// Bind textures to shader resource slots
	ID3D11ShaderResourceView* textures[] = {
		myTextureManager->GetTexture("Default"),
		myTextureManager->GetTexture("Grass_c"),
		myTextureManager->GetTexture("Rock_c"),
		myTextureManager->GetTexture("Snow_c"),
		myTextureManager->GetTexture("Grass_n"),
		myTextureManager->GetTexture("Rock_n"),
		myTextureManager->GetTexture("Snow_n"),
		myTextureManager->GetTexture("Grass_m"),
		myTextureManager->GetTexture("Rock_m"),
		myTextureManager->GetTexture("Snow_m"),
		myTextureManager->GetTexture("Noise"),
		myTextureManager->GetTexture("Cubemap"),
		reflectionSRV // Reflection texture bound here if available
	};

	// Ensure reflection SRV is bound only when valid
	if (!reflectionSRV)
	{
		textures[12] = nullptr;
	}

	myContext->PSSetShaderResources(0, _countof(textures), textures);
}
bool GraphicsEngine::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2; // Double buffering
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = *myWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	UINT creationFlags = 0;

#if defined(REPORT_DX_WARNINGS)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	if (FAILED
	(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mySwapChain,
		&myDevice,
		nullptr,
		&myContext)))
	{
		return false;
	}

	return true;
}
bool GraphicsEngine::CreateRenderTarget(HRESULT& aHresult)
{
	ID3D11Texture2D* backBufferTexture{};
	aHresult = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture));
	if (FAILED(aHresult))
	{
		return false;
	}

	aHresult = myDevice->CreateRenderTargetView(backBufferTexture, nullptr, &myBackBuffer);
	if (FAILED(aHresult))
	{
		return false;
	}
	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());

	D3D11_TEXTURE2D_DESC textureDesc;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();
	myBackBufferTextureHeight = textureDesc.Height;
	myBackBufferTextureWidth = textureDesc.Width;
	return true;
}
bool GraphicsEngine::CreateReflectionRenderTarget(HRESULT& aHresult)
{
	for (int i = 0; i < 2; i++) 
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = myBackBufferTextureWidth;
		textureDesc.Height = myBackBufferTextureHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		aHresult = myDevice->CreateTexture2D(&textureDesc, nullptr, &myReflectionTexture[i]);
		if (FAILED(aHresult))
		{
			return false;
		}
		aHresult = myDevice->CreateRenderTargetView(myReflectionTexture[i].Get(), nullptr, &myReflectionRTV[i]);
		if (FAILED(aHresult))
		{
			return false;
		}
		aHresult = myDevice->CreateShaderResourceView(myReflectionTexture[i].Get(), nullptr, &myReflectionSRV[i]);
		if (FAILED(aHresult))
		{
			return false;
		}
	}
	return true;
}
bool GraphicsEngine::CreateDepthStencilState(HRESULT& aHresult) const
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE; // Disable stencil testing

	ID3D11DepthStencilState* depthStencilState;
	aHresult = myDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(aHresult))
	{
		return false;
	}

	myContext->OMSetDepthStencilState(depthStencilState, 0);

	depthStencilState->Release();
	return true;
}
bool GraphicsEngine::CreateDepthBuffer(HRESULT& aHresult)
{
	// Create depth buffer texture
	ID3D11Texture2D* depthBufferTexture;
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = static_cast<unsigned int>(myBackBufferTextureWidth);
	depthBufferDesc.Height = static_cast<unsigned int>(myBackBufferTextureHeight);
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	aHresult = myDevice->CreateTexture2D(&depthBufferDesc, nullptr, &depthBufferTexture);
	if (FAILED(aHresult))
	{
		return false;
	}

	// Create depth stencil view
	aHresult = myDevice->CreateDepthStencilView(depthBufferTexture, nullptr, &myDepthBuffer);
	if (FAILED(aHresult))
	{
		depthBufferTexture->Release();
		return false;
	}

	// Release depth buffer texture (optional, depends on usage)
	//depthBufferTexture->Release();

	return true;
}
bool GraphicsEngine::CreateFrameAndObjectBuffer(HRESULT& aHresult)
{
	{ // constant buffers, framebuffer & objectbuffer
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		bufferDesc.ByteWidth = sizeof(FrameBufferData);
		aHresult = myDevice->CreateBuffer(&bufferDesc, nullptr, &myFrameBuffer);
		if (FAILED(aHresult))
		{
			return false;
		}

		bufferDesc.ByteWidth = sizeof(ObjectBufferData);
		aHresult = myDevice->CreateBuffer(&bufferDesc, nullptr, &myObjectBuffer);
		if (FAILED(aHresult))
		{
			return false;
		}

		// assuming the layout doesn't change
		myContext->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
		myContext->PSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
		myContext->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
	}
	return true;
}
bool GraphicsEngine::CreateLightBuffer(HRESULT& aHresult)
{
	D3D11_BUFFER_DESC lightBufferDesc = {};
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	aHresult = myDevice->CreateBuffer(&lightBufferDesc, nullptr, &myLightBuffer);
	if (FAILED(aHresult))
	{
		return false;
	}

	myContext->VSSetConstantBuffers(2, 1, myLightBuffer.GetAddressOf());
	myContext->PSSetConstantBuffers(2, 1, myLightBuffer.GetAddressOf());

	return true;
}
bool GraphicsEngine::CreateTextureSamplerState(HRESULT& aHresult)
{
	{
		D3D11_SAMPLER_DESC samplerDesc{};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		aHresult = myDevice->CreateSamplerState(&samplerDesc, &mySamplerState);
		if (FAILED(aHresult))
		{
			return false;
		}
		myContext->PSSetSamplers(0, 1, mySamplerState.GetAddressOf()); // Assuming slot 0

	}
	return true;
}
bool GraphicsEngine::CreateRasterizerState(HRESULT& aHresult)
{
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = TRUE; // Ensure this is set to TRUE for standard counter-clockwise winding
	rasterDesc.DepthClipEnable = TRUE;

	aHresult = myDevice->CreateRasterizerState(&rasterDesc, &myRasterizerState);
	if (FAILED(aHresult))
	{
		return false;
	}

	// Define the rasterizer description for front face culling
	rasterDesc.CullMode = D3D11_CULL_FRONT; // Cull front faces

	aHresult = myDevice->CreateRasterizerState(&rasterDesc, &myRasterizerStateFrontFaceCulling);
	if (FAILED(aHresult))
	{
		return false;
	}

	// Define the rasterizer description for wireframe
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.CullMode = D3D11_CULL_NONE;      // Disable culling for full visibility
	rasterDesc.DepthClipEnable = TRUE;

	aHresult = myDevice->CreateRasterizerState(&rasterDesc, &myRasterizerStateWireframe);
	if (FAILED(aHresult))
	{
		return false;
	}

	myContext->RSSetState(myRasterizerState.Get());
	return true;
}
bool GraphicsEngine::CreateBlendState(HRESULT& aHresult)
{
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	aHresult = myDevice->CreateBlendState(&blendDesc, &myBlendState);
	if (FAILED(aHresult))
	{
		return false;
	}

	return true;
}
void GraphicsEngine::SetBlendState()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	myContext->OMSetBlendState(myBlendState.Get(), blendFactor, sampleMask);
}
bool GraphicsEngine::CreateCamera(float aWidth, float aHeight)
{
	//myCamera = std::make_shared<Camera>({ 0.0f, 0.0f, -50.0f }, { 0.0f, 0.0f, 0.0f });

	myCamera = std::make_unique<Camera>();
	myCamera->SetTransform({ 0.0f, 0.0f, -50.0f }, { 0.0f, 0.0f, 0.0f });
	myCamera->SetAspectRatio({aWidth, aHeight});
	myCamera->SetFOV(90.0f);
	myCamera->SetNearPlane(0.01f);
	myCamera->SetFarPlane(1000.0f);
	myCamera->UpdatePerspectiveProjection();

	return true;
}
void GraphicsEngine::SetupViewport() const
{
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(myBackBufferTextureWidth);
	viewport.Height = static_cast<float>(myBackBufferTextureHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	myContext->RSSetViewports(1, &viewport);
}
bool GraphicsEngine::CompileShaders(const std::wstring& shaderFolder)
{
	if (!std::filesystem::exists(shaderFolder))
	{
		std::cerr << "Shader folder does not exist: " << WStringToString(shaderFolder) << std::endl;
		return false;
	}

	for (const auto& entry : std::filesystem::directory_iterator(shaderFolder))
	{
		if (entry.is_regular_file())
		{
			std::wstring shaderFilePath = entry.path().wstring();
			std::wstring extension = entry.path().extension().wstring();

			// Check if the file is a .hlsl file
			if (extension != L".hlsl")
			{
				continue;
			}

			// Generate the output file path by replacing the .hlsl extension with .cso
			std::wstring outputFilePath = shaderFilePath;
			outputFilePath.replace(outputFilePath.find(L".hlsl"), 5, L".cso");

			std::string entryPoint = "main";
			std::string target;

			if (shaderFilePath.find(L"_VS.hlsl") != std::wstring::npos)
			{
				target = "vs_5_0";
			}
			else if (shaderFilePath.find(L"_PS.hlsl") != std::wstring::npos)
			{
				target = "ps_5_0";
			}
			else
			{
				//std::cerr << "Unknown shader type for file: " << WStringToString(shaderFilePath) << std::endl;
				continue;
			}

			UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
			compileFlags |= D3DCOMPILE_DEBUG;
#endif

			ID3DBlob* shaderBlob = nullptr;
			ID3DBlob* errorBlob = nullptr;

			HRESULT hr = D3DCompileFromFile(shaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), compileFlags, 0, &shaderBlob, &errorBlob);

			if (FAILED(hr))
			{
				if (errorBlob)
				{
					//std::cerr << "Shader compilation error: " << static_cast<const char*>(errorBlob->GetBufferPointer()) << std::endl;
					errorBlob->Release();
				}
				if (shaderBlob)
				{
					shaderBlob->Release();
				}
				return false;
			}

			std::ofstream outputFile(outputFilePath, std::ios::binary);
			if (!outputFile)
			{
				//std::cerr << "Failed to open output file: " << WStringToString(outputFilePath) << std::endl;
				shaderBlob->Release();
				return false;
			}

			outputFile.write(static_cast<const char*>(shaderBlob->GetBufferPointer()), shaderBlob->GetBufferSize());
			outputFile.close();

			shaderBlob->Release();
		}
	}

	return true;
}
void GraphicsEngine::PrintDebugMessages()
{
	// Obtain info queue
	myDevice->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&myDebugInfoQueue);
	// Pushes a filter that doesn't filter out any messages
	myDebugInfoQueue->PushEmptyStorageFilter();

	UINT64 message_count = myDebugInfoQueue->GetNumStoredMessages();
	for (UINT64 i = 0; i < message_count; i++)
	{
		SIZE_T message_size = 0;
		myDebugInfoQueue->GetMessage(i, nullptr, &message_size); //get the size of the message

		D3D11_MESSAGE* message = (D3D11_MESSAGE*)malloc(message_size); //allocate enough space
		/*myHResult(*/myDebugInfoQueue->GetMessage(i, message, &message_size); //get the actual message

		//do whatever you want to do with it
		printf("Directx11: %.*s", static_cast<int>(message->DescriptionByteLength), message->pDescription);

		free(message);
	}
	myDebugInfoQueue->ClearStoredMessages();
}
void GraphicsEngine::CleanupAllExceptDevice()
{
	myObjectsToRender.clear();

	if (myTextureManager)
	{
		myTextureManager.reset();
	}

	if (myCamera)
	{
		myCamera.reset();
	}

	if (mySamplerState) mySamplerState.Reset();
	if (myObjectBuffer) myObjectBuffer.Reset();
	if (myFrameBuffer) myFrameBuffer.Reset();
	if (myDepthBuffer) myDepthBuffer.Reset();
	if (myBackBuffer) myBackBuffer.Reset();
	for (int i = 0; i < 2; i++) 
	{
		if (myReflectionTexture[i]) myReflectionTexture[i].Reset();
		if (myReflectionRTV[i]) myReflectionRTV[i].Reset();
		if (myReflectionSRV[i]) myReflectionSRV[i].Reset();
	}
	if (mySwapChain) mySwapChain.Reset();
	if (myContext)
	{
		myContext->ClearState(); // Clear all bound resources
		myContext->Flush(); // Ensure all commands are finished
		myContext.Reset();
	}
}
void GraphicsEngine::ReportLiveObjects()
{
	ComPtr<ID3D11Debug> debugDevice;
	if (SUCCEEDED(GetDevice()->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(debugDevice.GetAddressOf()))))
	{
		debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL); // Report live objects to check for memory leaks
	}
}
void GraphicsEngine::CleanupDevice()
{
	if (myDevice) myDevice.Reset();
}
void GraphicsEngine::CleanupInfoQueue()
{
	if (myDebugInfoQueue) myDebugInfoQueue.Reset();
}

//bool GraphicsEngine::IsChunkInView(const BoundingBox& chunkBounds, const Frustum& viewFrustum)
//{
//	// Check if the chunk's bounding box intersects the view frustum
//	return viewFrustum.Intersects(chunkBounds);
//}
//
//void GraphicsEngine::RenderTerrain(const std::vector<TerrainChunk>& terrainChunks, const Frustum& viewFrustum)
//{
//	for (const auto& chunk : terrainChunks)
//	{
//		if (IsChunkInView(chunk.bounds, viewFrustum))
//		{
//			// Render the chunk
//			chunk.Render();
//		}
//	}
//}
//void GraphicsEngine::RenderTerrain(const std::vector<TerrainChunk>& terrainChunks, const Camera& camera)
//{
//	for (const auto& chunk : terrainChunks)
//	{
//		float distance = (chunk.position - camera.position).Length();
//		if (distance < LOD1_DISTANCE)
//		{
//			chunk.RenderHighDetail();
//		}
//		else if (distance < LOD2_DISTANCE)
//		{
//			chunk.RenderMediumDetail();
//		}
//		else
//		{
//			chunk.RenderLowDetail();
//		}
//	}
//}