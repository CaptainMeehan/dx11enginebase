#include "OtherForm.h"
#include <d3d11.h>
#include <fstream>
#include "Vertex.h"
bool OtherForm::Initialize(ID3D11Device* device)
{
	HRESULT result;
	float startCoordX1 = 0.3f;
	float startCoordY1 = 0.3f;
	float startCoordX2 = -0.3f;
	float startCoordY2 = 0.3f;
	float startCoordX3 = -0.3f;
	float startCoordY3 = -0.3f;
	float startCoordX4 = 0.3f;
	float startCoordY4 = -0.3f;

	Vertex vertices[12] =
	{
		// First triangle (top-left)
		{ startCoordX1, startCoordY1, 0, 1, 1, 0, 0, 1 },
		{ startCoordX2, startCoordY2, 0, 1, 0, 1, 0, 1 },
		{ startCoordX3, startCoordY3, 0, 1, 0, 0, 1, 1 },

		// Second triangle (bottom-right)
		{ startCoordX1, startCoordY1, 0, 1, 1, 0, 0, 1 },
		{ startCoordX3, startCoordY3, 0, 1, 0, 0, 1, 1 },
		{ startCoordX4, startCoordY4, 0, 1, 0, 1, 0, 1 },

		// Third triangle (top-left)
		{ startCoordX1, startCoordY1-0.6f, 0, 1, 1, 0, 0, 1 },
		{ startCoordX2, startCoordY2 - 0.6f, 0, 1, 0, 1, 0, 1 },
		{ startCoordX3, startCoordY3 - 0.6f, 0, 1, 0, 0, 1, 1 },

		// Fourth triangle (bottom-right)
		{ startCoordX1, startCoordY1 - 0.6f, 0, 1, 1, 0, 0, 1 },
		{ startCoordX3, startCoordY3 - 0.6f, 0, 1, 0, 0, 1, 1 },
		{ startCoordX4, startCoordY4 - 0.6f, 0, 1, 0, 1, 0, 1 }
	};

	unsigned int indices[12] =
	{
		0, 2, 1, // First triangle
		3, 5, 4,  // Second triangle
		6, 8, 7, // First triangle
		9, 11, 10  // Second triangle
	};
	{
		// Create vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.ByteWidth = sizeof(vertices);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vertexData = { 0 };
		vertexData.pSysMem = vertices;
		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}
	{
		// Create index buffer
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * 12;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexData = { 0 };
		indexData.pSysMem = indices;
		// Create the index buffer.
		result = device->CreateBuffer(&indexBufferDesc, &indexData,
			&myIndexBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}
	std::string vsData;
	{
		// Load shaders
		std::ifstream vsFile;
		vsFile.open("OtherForm_VS.cso", std::ios::binary);
		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		result = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
		if (FAILED(result))
		{
			return false;
		}
		vsFile.close();
		std::ifstream psFile;
		psFile.open("OtherForm_PS.cso", std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
		if (FAILED(result))
		{
			return false;
		}
		psFile.close();
	}
	{
		// Create input layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		result = device->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &myInputLayout);
		if (FAILED(result))
		{
			return false;
		}
	}
	return true;
}
void OtherForm::Render(ID3D11DeviceContext* context)
{
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(myInputLayout.Get());
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->VSSetShader(myVertexShader.Get(), nullptr, 0);
	context->PSSetShader(myPixelShader.Get(), nullptr, 0);
	context->DrawIndexed(12, 0, 0);
}