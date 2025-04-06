#include "Texture2D.h"

bool Texture2D::Initialize(ID3D11Device* aDevice, unsigned char* someRGBAPixels, int aWidth, int aHeight)
{
	ID3D11Texture2D* texture = nullptr;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA subresourceDesc = {};
	subresourceDesc.pSysMem = (void*)someRGBAPixels;
	subresourceDesc.SysMemPitch = aWidth * 4;
	subresourceDesc.SysMemSlicePitch = aWidth * aHeight * 4;
	if (FAILED(aDevice->CreateTexture2D(&desc, &subresourceDesc, &texture)))
		return false;
	HRESULT hr = aDevice->CreateShaderResourceView(texture, NULL, &myShaderResourceView);
	if (FAILED(hr))
	{
		return false;
	}
	texture->Release();
	return true;
}

void Texture2D::Bind(ID3D11DeviceContext* aContext, int aSlot)
{

	aContext->PSSetShaderResources(0, 1, myShaderResourceView.GetAddressOf());

}

ID3D11ShaderResourceView* Texture2D::GetShaderResourceView()
{
	return myShaderResourceView.Get();
}
