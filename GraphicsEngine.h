#pragma once
#include <wrl/client.h>
#include <d3d11.h>

#include "LightBufferData.hpp"
#include "Includes/MeehanVector2.hpp"
#include "Sphere.h"
#include "Pyramid.h"
#include "Terrain.h"
#include "TextureManager.h"
#include "Includes/Camera.h"

using Microsoft::WRL::ComPtr;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

class GraphicsEngine
{
public:
	GraphicsEngine() = default;
	~GraphicsEngine() = default;
	GraphicsEngine(const GraphicsEngine&) = delete;
	GraphicsEngine& operator=(const GraphicsEngine&) = delete;
	GraphicsEngine(const GraphicsEngine&&) = delete;
	GraphicsEngine& operator=(const GraphicsEngine&&) = delete;

	bool Init(int aHeight, int aWidth, HWND aWindowHandle);
	void Update(float aDeltaTime, CommonUtilities::InputHandler& anInputHandler);
	void Render(float aDeltaTime);
	void RenderScene(float aDeltaTime, bool isReflection);
	void BindTextures(ID3D11ShaderResourceView* reflectionSRV);
	bool CreateDeviceAndSwapChain();
	bool CreateRenderTarget(HRESULT& aHresult);
	bool CreateDepthStencilState(HRESULT& aHresult) const;
	bool CreateDepthBuffer(HRESULT& aHresult);
	bool CreateFrameAndObjectBuffer(HRESULT& aHresult);
	bool CreateLightBuffer(HRESULT& aHresult);
	bool CreateTextureSamplerState(HRESULT& aHresult);
	bool CreateRasterizerState(HRESULT& aHresult);
	bool CreateBlendState(HRESULT& aHresult);
	void SetBlendState();
	bool CreateReflectionRenderTarget(HRESULT& aHresult);
	bool CreateCamera(float aWidth, float aHeight);
	void SetupViewport() const;
	void UpdateFrameBuffer(float aDeltaTime);
	void UpdateFrameBufferForReflection();
	void UpdateObjectBuffer(const std::shared_ptr<Object3D>& anObject) const;
	void UpdateLightBuffer();
	void UpdateTimeOfDay();
	void RenderObjects(bool isReflection);
	bool CompileShaders(const std::wstring& shaderFolder);
	void PrintDebugMessages();
	void CleanupAllExceptDevice();
	void ReportLiveObjects();
	void CleanupDevice();
	void CleanupInfoQueue();
	CommonUtilities::Vector2<unsigned int> GetWindowSize() const { return myWindowSize; }
	CommonUtilities::Vector2<float> GetRenderSize() const { return myRenderSize; }
	unsigned int Align16(unsigned int aSize) { return ((aSize + 15) / 16) * 16; } // Aligns the size to 16 bytes

	ID3D11Device* GetDevice() const { return myDevice.Get(); }
	ID3D11DeviceContext* GetContext() const { return myContext.Get(); }
	IDXGISwapChain* GetSwapChain() const { return mySwapChain.Get(); }
	ID3D11RenderTargetView* GetBackBuffer() const { return myBackBuffer.Get(); }
	ID3D11DepthStencilView* GetDepthBuffer() const { return myDepthBuffer.Get(); }
	ID3D11Buffer* GetFrameBuffer() const { return myFrameBuffer.Get(); }
	ID3D11Buffer* GetObjectBuffer() const { return myObjectBuffer.Get(); }
	Camera& GetCamera() { return *myCamera; }
	Sphere& GetSphere() { return *mySphere; }
	TextureManager& GetTextureManager() const { return *myTextureManager; }
	const double& GetTimeOfDay() const { return myTimeOfDay; }


private:
	std::unique_ptr<TextureManager> myTextureManager;

	HRESULT myHResult;
	float myElapsedTime = 0.0f;
	ComPtr<ID3D11InfoQueue> myDebugInfoQueue;
	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	ComPtr<IDXGISwapChain> mySwapChain;
	ComPtr<ID3D11RenderTargetView> myBackBuffer;
	ComPtr<ID3D11DepthStencilView> myDepthBuffer;
	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11Buffer> myObjectBuffer;
	ComPtr<ID3D11Buffer> myLightBuffer;
	ComPtr<ID3D11SamplerState> mySamplerState;
	ComPtr<ID3D11RasterizerState> myRasterizerState;
	ComPtr<ID3D11RasterizerState> myRasterizerStateWireframe;
	ComPtr<ID3D11RasterizerState> myRasterizerStateFrontFaceCulling;
	ComPtr<ID3D11BlendState> myBlendState;
	int myCurrentReflectionBufferIndex;
    ComPtr<ID3D11Texture2D> myReflectionTexture[2];
	ComPtr<ID3D11RenderTargetView> myReflectionRTV[2];
	ComPtr<ID3D11ShaderResourceView> myReflectionSRV[2];
	std::unique_ptr<Camera> myCamera;
	std::unique_ptr<Camera> mySavedCamera;
	std::shared_ptr<Sphere> mySphere;
	CommonUtilities::Vector2<unsigned int> myWindowSize;
	CommonUtilities::Vector2<float> myRenderSize;
	HWND* myWindowHandle;
	UINT myBackBufferTextureHeight;
	UINT myBackBufferTextureWidth;

	float myClearColor[4] = { 0.68f, 0.85f, 0.90f, 1.0f }; // Light blue
	std::vector<std::shared_ptr<Object3D>> myObjectsToRender;
	double myTimeOfDay = {};
	int myRenderMode = 0;
	std::shared_ptr<Terrain> myTerrain;
	float myReflectionPlaneHeight;
};