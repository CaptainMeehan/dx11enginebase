#include "TextureManager.h"

#include <iostream>

#include "Includes/External/stb_image.h"
#include <vector>

#include "Texture2D.h"
#include "Includes/PCG/PerlinNoise.hpp"
#include "Includes/External/DDSTextureLoader/DDSTextureLoader11.h"
#include <d3d11.h>
#include <wrl/client.h>

#include "StringConversion.hpp"

using Microsoft::WRL::ComPtr;

std::vector<float> GenerateNoiseTextureData(int width, int height, const siv::PerlinNoise& perlin, float scale)
{
    std::vector<float> noiseData(width * height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            float fx = static_cast<float>(x) / width * scale;
            float fy = static_cast<float>(y) / height * scale;
            noiseData[y * width + x] = static_cast<float>(perlin.noise2D_01(fx, fy));
        }
    }

    return noiseData;
}

ComPtr<ID3D11Texture2D> CreateNoiseTexture(ID3D11Device* device, const std::vector<float>& noiseData, int width, int height)
{
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = noiseData.data();
    initData.SysMemPitch = width * sizeof(float);

    ComPtr<ID3D11Texture2D> noiseTexture;
    HRESULT hr = device->CreateTexture2D(&textureDesc, &initData, &noiseTexture);
    if (FAILED(hr))
    {
        return nullptr;
    }

    return noiseTexture;
}

ComPtr<ID3D11ShaderResourceView> CreateNoiseTextureSRV(ID3D11Device* device, ID3D11Texture2D* noiseTexture)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    ComPtr<ID3D11ShaderResourceView> noiseTextureSRV;
    HRESULT hr = device->CreateShaderResourceView(noiseTexture, &srvDesc, &noiseTextureSRV);
    if (FAILED(hr))
    {
        return nullptr;
    }

    return noiseTextureSRV;
}
bool TextureManager::Init(ID3D11Device* aDevice, ID3D11DeviceContext* aContext)
{
    myDevice = aDevice;
    myContext = aContext;
    if (!LoadDefaultTexture())
    {
        return false;
    }
    if (!LoadTexture("Grass_c", "uppgift06texturer/Grass_c.png"))
    {
        return false;
    }
    if (!LoadTexture("Grass_n", "uppgift06texturer/Grass_n.png"))
    {
        return false;
    }
    if (!LoadTexture("Grass_m", "uppgift06texturer/Grass_m.png"))
    {
        return false;
    }
    if (!LoadTexture("Rock_c", "uppgift06texturer/Rock_c.png"))
    {
        return false;
    }
    if (!LoadTexture("Rock_n", "uppgift06texturer/Rock_n.png"))
    {
        return false;
    }
    if (!LoadTexture("Rock_m", "uppgift06texturer/Rock_m.png"))
    {
        return false;
    }
    if (!LoadTexture("Snow_c", "uppgift06texturer/Snow_c.png"))
    {
        return false;
    }
    if (!LoadTexture("Snow_n", "uppgift06texturer/Snow_n.png"))
    {
        return false;
    }
    if (!LoadTexture("Snow_m", "uppgift06texturer/Snow_m.png"))
    {
        return false;
    }
    if (!GenerateAndLoadNoiseTexture("Noise", 256, 256, 10.0f))
    {
        return false;
    }
    if (!LoadCubemap("Cubemap", "uppgift06texturer/cubemap.dds"))
    {
        return false;
    }

    return true;
}
bool TextureManager::LoadTexture(const std::string& aName, const std::string& aFilename)
{
    int width, height, channels;
    std::string filepath = std::string(SOLUTION_DIR) + "TGP/Textures/" + aFilename;

    // Check if filename contains _n for normal map
    bool useSRGB = true;
    bool isNormalMap = aFilename.find("_n") != std::string::npos;
    if (isNormalMap)
    {
        useSRGB = false;
    }

    unsigned char* img = stbi_load(filepath.c_str(), &width, &height, &channels, 4); // Force RGBA
    if (!img) {
        std::cerr << "Failed to load texture: " << aFilename << "\n";
        return false;
    }

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.MipLevels = 0; // Allow for automatic mipmap generation
    textureDesc.ArraySize = 1;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    if (useSRGB)
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    else
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;

    ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = myDevice->CreateTexture2D(&textureDesc, nullptr, &texture);
    if (FAILED(hr)) {
        stbi_image_free(img);
        return false;
    }

    // Update the texture with the loaded image data
    myContext->UpdateSubresource(texture.Get(), 0, nullptr, img, width * 4, 0);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = -1;

    ComPtr<ID3D11ShaderResourceView> srv;
    hr = myDevice->CreateShaderResourceView(texture.Get(), &srvDesc, &srv);
    if (FAILED(hr)) {
        stbi_image_free(img);
        return false;
    }

    // Generate mipmaps
    ComPtr<ID3D11DeviceContext> context;
    myDevice->GetImmediateContext(&context);
    context->GenerateMips(srv.Get());

    myTextures[aName] = srv;

    stbi_image_free(img); // Free the image data
    return true;
}
bool TextureManager::GenerateAndLoadNoiseTexture(const std::string& aName, int width, int height, float scale)
{
    siv::PerlinNoise perlin(123456); // Seed for reproducibility
    std::vector<float> noiseData = GenerateNoiseTextureData(width, height, perlin, scale);

    ComPtr<ID3D11Texture2D> noiseTexture = CreateNoiseTexture(myDevice, noiseData, width, height);
    if (!noiseTexture)
    {
        //std::cerr << "Failed to create noise texture" << std::endl;
        return false;
    }

    ComPtr<ID3D11ShaderResourceView> noiseTextureSRV = CreateNoiseTextureSRV(myDevice, noiseTexture.Get());
    if (!noiseTextureSRV)
    {
        //std::cerr << "Failed to create shader resource view for noise texture" << std::endl;
        return false;
    }

    myTextures[aName] = noiseTextureSRV;
    return true;
}
ID3D11ShaderResourceView* TextureManager::GetTexture(const std::string& aName) const
{
    auto it = myTextures.find(aName);
    if (it != myTextures.end()) 
    {
        return it->second.Get();
    }
    return nullptr; // Return nullptr if texture is not found
}
bool TextureManager::LoadCubemap(const std::string& aName, const std::string& aFilename)
{
    std::wstring filepath = StringToWString(std::string(SOLUTION_DIR) + "TGP/Textures/" + aFilename);
    ComPtr<ID3D11ShaderResourceView> cubemapSRV;
    HRESULT result = DirectX::CreateDDSTextureFromFile(myDevice, filepath.c_str(), nullptr, &cubemapSRV);
    if (FAILED(result))
    {
        return false;
    }

    myTextures[aName] = cubemapSRV;
    return true;
}
bool TextureManager::LoadDefaultTexture()
{
    unsigned char uniqueWhitePixel[4] = { 254, 254, 254, 255 }; // Unique color to check for on shader
    Texture2D defaultTexture;
    if (!defaultTexture.Initialize(myDevice, uniqueWhitePixel, 1, 1)) {
        return false;
    }
    myTextures["Default"] = defaultTexture.GetShaderResourceView();
    return true;
}