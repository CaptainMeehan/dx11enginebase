#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <vector>
#include <string>
#include "Includes/Matrix4x4.h"
#include "Vertex.h"

using Microsoft::WRL::ComPtr;

class Object3D
{
public:
    Object3D() = default;
    virtual ~Object3D() = default;

    virtual bool Initialize(ID3D11Device* aDevice);
    virtual bool InitObjectResources() = 0;
    virtual void Render(ID3D11DeviceContext* aContext);

    void SetPosition(const CommonUtilities::Vector3<float>& aPosition);
    void SetRotation(const CommonUtilities::Vector3<float>& aRotation);
    void SetScale(const CommonUtilities::Vector3<float>& aScale);

    const CommonUtilities::Matrix4x4<float>& GetModelToWorld() const;
    CommonUtilities::Matrix4x4<float> GetWorldMatrix();
    CommonUtilities::Vector3<float> GetNormal() const;

    void SetTexture(ID3D11ShaderResourceView* aTexture);
    ID3D11ShaderResourceView* const* GetTexture() const;
    std::string GetVertexShaderPath() const;
    std::string GetPixelShaderPath() const;
	void SetVertexShaderPath(const std::string& aPath);
	void SetPixelShaderPath(const std::string& aPath);

protected:
    virtual void CreateGeometry(std::vector<Vertex>& vertices, std::vector<UINT>& indices) = 0;

    bool LoadShadersAndCreateInputLayout(ID3D11Device* aDevice);
    bool CreateBuffers(ID3D11Device* aDevice, const std::vector<Vertex>& someVertices, const std::vector<UINT>& someIndices);

    ComPtr<ID3D11Buffer> myLightBuffer;
    ComPtr<ID3D11Buffer> myVertexBuffer;
    ComPtr<ID3D11Buffer> myIndexBuffer;
    ComPtr<ID3D11VertexShader> myVertexShader;
    ComPtr<ID3D11PixelShader> myPixelShader;
    ComPtr<ID3D11InputLayout> myInputLayout;
    ComPtr<ID3D11ShaderResourceView> myTexture;
    unsigned int myIndexCount = 0;

	std::string myVertexShaderPath;
	std::string myPixelShaderPath;

    CommonUtilities::Matrix4x4<float> myTransformationMatrix;
    CommonUtilities::Matrix4x4<float> myWorldMatrix;
    CommonUtilities::Vector3<float> myPosition = { 0.0f, 0.0f, 0.0f };
    CommonUtilities::Vector3<float> myRotation = { 0.0f, 0.0f, 0.0f };
    CommonUtilities::Vector3<float> myScale = { 1.0f, 1.0f, 1.0f };
    CommonUtilities::Vector3<float> myNormal;

private:
    void UpdateTransformationMatrix();
    void UpdateWorldMatrix();
};