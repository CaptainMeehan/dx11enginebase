#pragma once
#include <d3d11.h>
#include <map>
#include <string>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class Texture2D
{
public:
	Texture2D() = default;
	~Texture2D() = default;
	bool Initialize(ID3D11Device* aDevice, unsigned char* someRGBAPixels, int aWidth, int aHeight);
	void Bind(ID3D11DeviceContext* aContext, int aSlot);
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
};
