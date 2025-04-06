#include "Sphere.h"
#include <directxmath.h>

#include "Engine.h"
#include "GraphicsEngine.h"

void Sphere::CreateGeometry(std::vector<Vertex>& someVertices, std::vector<UINT>& someIndices)
{
    const int latitudeSegments = 20;
    const int longitudeSegments = 20;
    const float radius = 1.0f;

    for (int i = 0; i <= latitudeSegments; ++i)
    {
        float theta = i * DirectX::XM_PI / latitudeSegments;
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);

        for (int j = 0; j <= longitudeSegments; ++j)
        {
            float phi = j * 2.0f * DirectX::XM_PI / longitudeSegments;
            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            DirectX::XMFLOAT3 position = {
                radius * sinTheta * cosPhi,
                radius * cosTheta,
                radius * sinTheta * sinPhi
            };

            DirectX::XMFLOAT2 texcoord = {
                static_cast<float>(j) / longitudeSegments,
                static_cast<float>(i) / latitudeSegments
            };

            someVertices.push_back({ position.x, position.y, position.z, 1.0f, 1.0f, 1.0f, 1.0f, texcoord.x, texcoord.y });
        }
    }

    for (int i = 0; i < latitudeSegments; ++i)
    {
        for (int j = 0; j < longitudeSegments; ++j)
        {
            int first = (i * (longitudeSegments + 1)) + j;
            int second = first + longitudeSegments + 1;

            someIndices.push_back(first);
            someIndices.push_back(first + 1);
            someIndices.push_back(second);

            someIndices.push_back(second);
            someIndices.push_back(first + 1);
            someIndices.push_back(second + 1);
        }
    }
}
bool Sphere::InitObjectResources()
{
    TextureManager& textureManager = Engine::GetInstance().GetGraphicsEngine().GetTextureManager();

    SetVertexShaderPath("Sphere_VS.cso");
    SetPixelShaderPath("Sphere_PS.cso");

    if (!textureManager.LoadTexture("Sphere", "sphere.jpg"))
    {
        return false;
    }
    SetTexture(textureManager.GetTexture("Sphere"));

	return true;
}