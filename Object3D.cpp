#include "Object3D.h"

#include <filesystem>
#include <fstream>

#include "Engine.h"
#include "GraphicsEngine.h"
#include "TextureManager.h"
#include "Vertex.h"

bool Object3D::Initialize(ID3D11Device* aDevice)
{
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
    CreateGeometry(vertices, indices);
    InitObjectResources();
    if (!CreateBuffers(aDevice, vertices, indices))
    {
        return false;
    }
    return LoadShadersAndCreateInputLayout(aDevice);
}
void Object3D::Render(ID3D11DeviceContext* aContext)
{
    aContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    aContext->IASetInputLayout(myInputLayout.Get());
    unsigned int stride = sizeof(Vertex);
    unsigned int offset = 0;
    aContext->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
    aContext->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    aContext->VSSetShader(myVertexShader.Get(), nullptr, 0);
    aContext->PSSetShader(myPixelShader.Get(), nullptr, 0);
    if (myTexture)
    {
        aContext->PSSetShaderResources(0, 1, GetTexture());
    }
    /*else
    {
        aContext->PSSetShaderResources(0, 1, GetTexture());
    }*/
    aContext->DrawIndexed(myIndexCount, 0, 0);
}
bool Object3D::LoadShadersAndCreateInputLayout(ID3D11Device* aDevice)
{
    std::string vsData;
    std::ifstream vsFile;
    vsFile.open(GetVertexShaderPath(), std::ios::binary);
    if (!vsFile.is_open())
    {
        std::cerr << "Failed to open vertex shader file: " << GetVertexShaderPath() << std::endl;
        return false;
    }
    vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
    vsFile.close();

    HRESULT hr = aDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create vertex shader: " << GetVertexShaderPath() << " Error: " << std::hex << hr << std::endl;
        return false;
    }

    std::ifstream psFile;
    psFile.open(GetPixelShaderPath(), std::ios::binary);
    if (!psFile.is_open())
    {
        std::cerr << "Failed to open pixel shader file: " << GetPixelShaderPath() << std::endl;
        return false;
    }
    std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
    psFile.close();

    hr = aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create pixel shader: " << GetPixelShaderPath() << " Error: " << std::hex << hr << std::endl;
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = aDevice->CreateInputLayout(layout, _countof(layout), vsData.data(), vsData.size(), &myInputLayout);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create input layout for vertex shader: " << GetVertexShaderPath() << " Error: " << std::hex << hr << std::endl;
        return false;
    }
    return true;
}
bool Object3D::CreateBuffers(ID3D11Device* aDevice, const std::vector<Vertex>& someVertices, const std::vector<UINT>& someIndices)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA initData = {};

    // Vertex buffer
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * someVertices.size());
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    initData.pSysMem = someVertices.data();
    if (FAILED(aDevice->CreateBuffer(&bufferDesc, &initData, myVertexBuffer.GetAddressOf())))
    {
        return false;
    }

    // Index buffer
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * someIndices.size());
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    initData.pSysMem = someIndices.data();
    if (FAILED(aDevice->CreateBuffer(&bufferDesc, &initData, myIndexBuffer.GetAddressOf())))
    {
        return false;
    }

    myIndexCount = static_cast<unsigned int>(someIndices.size());
    return true;
}
void Object3D::UpdateTransformationMatrix()
{
    myTransformationMatrix = CommonUtilities::Matrix4x4<float>::CreateTranslationMatrix(myPosition.x, myPosition.y, myPosition.z) *
        CommonUtilities::Matrix4x4<float>::CreateRotation(myRotation.x, myRotation.y, myRotation.z) *
        CommonUtilities::Matrix4x4<float>::CreateScaleMatrix(myScale.x, myScale.y, myScale.z);
}
void Object3D::UpdateWorldMatrix()
{
    CommonUtilities::Matrix4x4 transMat = CommonUtilities::Matrix4x4<float>::CreateTranslationMatrix(myPosition.x, myPosition.y, myPosition.z);
    myWorldMatrix = transMat;
}

std::string Object3D::GetVertexShaderPath() const
{
    return myVertexShaderPath;
}
std::string Object3D::GetPixelShaderPath() const
{
    return myPixelShaderPath;
}
const CommonUtilities::Matrix4x4<float>& Object3D::GetModelToWorld() const
{
    return myTransformationMatrix;
}
CommonUtilities::Matrix4x4<float> Object3D::GetWorldMatrix()
{
    return myWorldMatrix;
}
ID3D11ShaderResourceView* const* Object3D::GetTexture() const
{
    return myTexture.GetAddressOf();
}
CommonUtilities::Vector3<float> Object3D::GetNormal() const
{
    return myNormal;
}
void Object3D::SetVertexShaderPath(const std::string& aPath)
{
    std::string solutionDir = SOLUTION_DIR;
    std::string shaderDir = "TGP/Shaders/";

    myVertexShaderPath = solutionDir + shaderDir + aPath;
}
void Object3D::SetPixelShaderPath(const std::string& aPath)
{
    std::string solutionDir = SOLUTION_DIR;
    std::string shaderDir = "TGP/Shaders/";

    myPixelShaderPath = solutionDir + shaderDir + aPath;
}
void Object3D::SetTexture(ID3D11ShaderResourceView* aTexture)
{
    myTexture = aTexture;
}
void Object3D::SetPosition(const CommonUtilities::Vector3<float>& aPos)
{
    myPosition = aPos;
    UpdateWorldMatrix();
}
void Object3D::SetRotation(const CommonUtilities::Vector3<float>& aRot)
{
    myRotation = aRot;
    UpdateTransformationMatrix();
}
void Object3D::SetScale(const CommonUtilities::Vector3<float>& aScl)
{
    myScale = aScl;
    UpdateTransformationMatrix();
}
