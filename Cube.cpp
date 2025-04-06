//#include "Cube.h"
//#include <vector>
//#include <cmath>
//#include <directxmath.h>
//#include <fstream>
//
//#include "Vertex.h"
//
//bool Cube::Initialize(ID3D11Device* aDevice)
//{
//    HRESULT result;
//    {
//        const int latitudeSegments = 20;
//        const int longitudeSegments = 20;
//        const float radius = 1.0f;
//
//        std::vector<Vertex> vertices;
//
//        DirectX::XMFLOAT4 color = { 1.0f, 0.0f, 0.0f, 1.0f }; // Red color
//
//        for (int i = 0; i <= latitudeSegments; ++i)
//        {
//            float theta = i * DirectX::XM_PI / latitudeSegments;
//            float sinTheta = sinf(theta);
//            float cosTheta = cosf(theta);
//
//            for (int j = 0; j <= longitudeSegments; ++j)
//            {
//                float phi = j * 2.0f * DirectX::XM_PI / longitudeSegments;
//                float sinPhi = sinf(phi);
//                float cosPhi = cosf(phi);
//
//                DirectX::XMFLOAT3 position = {
//                    radius * sinTheta * cosPhi,
//                    radius * cosTheta,
//                    radius * sinTheta * sinPhi
//                };
//
//                vertices.push_back({ position.x,position.y,position.z, color.x,color.y,color.z });
//            }
//        }
//
//        D3D11_BUFFER_DESC vertexBufferDesc = {};
//        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//        vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertices.size();
//        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//        vertexBufferDesc.CPUAccessFlags = 0;
//
//        D3D11_SUBRESOURCE_DATA vertexData = {};
//        vertexData.pSysMem = vertices.data();
//
//        HRESULT hr = aDevice->CreateBuffer(&vertexBufferDesc, &vertexData, myVertexBuffer.GetAddressOf());
//        if (FAILED(hr))
//        {
//            throw;
//            // Handle error
//        }
//    }
//    {
//        const int latitudeSegments = 20;
//        const int longitudeSegments = 20;
//
//        std::vector<UINT> indices;
//
//        for (int i = 0; i < latitudeSegments; ++i)
//        {
//            for (int j = 0; j < longitudeSegments; ++j)
//            {
//                int first = (i * (longitudeSegments + 1)) + j;
//                int second = first + longitudeSegments + 1;
//
//                indices.push_back(first);
//                indices.push_back(second);
//                indices.push_back(first + 1);
//
//                indices.push_back(second);
//                indices.push_back(second + 1);
//                indices.push_back(first + 1);
//            }
//        }
//        myIndexCount = indices.size();
//
//        D3D11_BUFFER_DESC indexBufferDesc = {};
//        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//        indexBufferDesc.ByteWidth = sizeof(UINT) * indices.size();
//        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//        indexBufferDesc.CPUAccessFlags = 0;
//
//        D3D11_SUBRESOURCE_DATA indexData = {};
//        indexData.pSysMem = indices.data();
//
//        HRESULT hr = aDevice->CreateBuffer(&indexBufferDesc, &indexData, myIndexBuffer.GetAddressOf());
//        if (FAILED(hr))
//        {
//            // Handle error
//        }
//    }
//    std::string vsData;
//    {
//        // Load shaders
//        std::ifstream vsFile;
//        vsFile.open("Sphere_VS.cso", std::ios::binary);
//        vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
//        result = aDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
//        if (FAILED(result))
//        {
//            return false;
//        }
//        vsFile.close();
//        std::ifstream psFile;
//        psFile.open("Sphere_PS.cso", std::ios::binary);
//        std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
//        result = aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
//        if (FAILED(result))
//        {
//            return false;
//        }
//        psFile.close();
//    }
//    {
//        D3D11_INPUT_ELEMENT_DESC layout[] =
//        {
//        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//        };
//        result = aDevice->CreateInputLayout(layout, 3, vsData.data(), vsData.size(), &myInputLayout);
//        if (FAILED(result))
//        {
//            return false;
//        }
//    }
//
//    return true;
//}
//void Cube::Render(ID3D11DeviceContext* aContext)
//{
//    aContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//    aContext->IASetInputLayout(myInputLayout.Get());
//    unsigned int stride = sizeof(Vertex);
//    unsigned int offset = 0;
//    aContext->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
//    aContext->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//    aContext->VSSetShader(myVertexShader.Get(), nullptr, 0);
//    aContext->PSSetShader(myPixelShader.Get(), nullptr, 0);
//    aContext->DrawIndexed(myIndexCount, 0, 0);
//}