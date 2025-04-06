#pragma once
#include <map>
#include <string>
#include <wrl/client.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class TextureManager
{
public:
    bool Init(ID3D11Device* aDevice, ID3D11DeviceContext* aContext);

    bool LoadTexture(const std::string& aName, const std::string& aFilename);
    bool GenerateAndLoadNoiseTexture(const std::string& aName, int width, int height, float scale);

    ID3D11ShaderResourceView* GetTexture(const std::string& aName) const;
    bool LoadCubemap(const std::string& aName, const std::string& aFilename);
    bool LoadDefaultTexture();

private:
    ID3D11Device* myDevice = nullptr;
    ID3D11DeviceContext* myContext = nullptr;
	std::map<std::string, ComPtr<ID3D11ShaderResourceView>> myTextures;
    };
