#include "Plane.h"

#include "Engine.h"
#include "GraphicsEngine.h"

void Plane::CreateGeometry(std::vector<Vertex>& someVertices, std::vector<UINT>& someIndices)
{
    // Define plane size (make it larger)
    float width = 1000.0f; // Adjust the size as needed
    float depth = 1000.0f; // Adjust the size as needed

    // Vertices
    someVertices = {
        { -width / 2, 0.0f, -depth / 2, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f }, // Bottom-left
        {  width / 2, 0.0f, -depth / 2, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }, // Bottom-right
        {  width / 2, 0.0f,  depth / 2, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f }, // Top-right
        { -width / 2, 0.0f,  depth / 2, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f }  // Top-left
    };

    // Indices
    someIndices = {
        0, 1, 2,
        0, 2, 3
    };

	CalculateNormal(someVertices);
}
void Plane::CalculateNormal(std::vector<Vertex>& someVertices)
{
    // Define two edges of the plane
    CommonUtilities::Vector3<float> edge1(someVertices[1].x - someVertices[0].x,
        someVertices[1].y - someVertices[0].y,
        someVertices[1].z - someVertices[0].z);

    CommonUtilities::Vector3<float> edge2(someVertices[3].x - someVertices[0].x,
        someVertices[3].y - someVertices[0].y,
        someVertices[3].z - someVertices[0].z);

    // Compute cross product
    myNormal = edge1.Cross(edge2);

    // Normalize the result to get the unit normal
    myNormal.Normalize();
}
bool Plane::InitObjectResources()
{
    TextureManager& textureManager = Engine::GetInstance().GetGraphicsEngine().GetTextureManager();

    SetVertexShaderPath("Terrain_VS.cso");
    SetPixelShaderPath("Plane_PS.cso");

    if (!textureManager.LoadTexture("Pyramid", "lattice.png"))
    {
        return false;
    }
    SetTexture(textureManager.GetTexture("Pyramid"));

    return true;
}
