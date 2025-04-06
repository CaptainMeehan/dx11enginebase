#pragma once
#include "Object3D.h"

class Terrain : public Object3D
{
public:
	Terrain() = default;
	~Terrain() = default;

	bool Initialize(ID3D11Device* aDevice) override;
	void Render(ID3D11DeviceContext* aContext) override;
	void RenderDiffuse(ID3D11DeviceContext* aContext);
	void RenderSpecular(ID3D11DeviceContext* aContext);
	void CreateGeometry(std::vector<Vertex>& vertices, std::vector<UINT>& indices) override;

	bool InitObjectResources() override;
	bool LoadTestShaders(ID3D11Device* aDevice);


	ComPtr<ID3D11PixelShader> myDiffusePixelShader;
	ComPtr<ID3D11PixelShader> mySpecularPixelShader;
	int myGridSize = 256;
	int myTileSize = 256;
};