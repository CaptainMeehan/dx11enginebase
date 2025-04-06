#include "Pyramid.h"

#include "Engine.h"
#include "GraphicsEngine.h"
#include "Vertex.h"

void Pyramid::CreateGeometry(std::vector<Vertex>& someVertices, std::vector<UINT>& someIndices)
{
    float baseSize = 1.0f; // Length of the base side
    float height = 1.0f;   // Height of the pyramid

    someVertices = {
        // Base vertices
        { -baseSize / 2, 0.0f, -baseSize / 2, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f }, // Bottom-left
        {  baseSize / 2, 0.0f, -baseSize / 2, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }, // Bottom-right
        {  baseSize / 2, 0.0f,  baseSize / 2, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f }, // Top-right
        { -baseSize / 2, 0.0f,  baseSize / 2, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, // Top-left

        // Apex
        { 0.0f, height, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f } // Apex
    };

    someIndices = {
        // Base
        0, 1, 2,
        0, 2, 3,

        // Sides
        0, 1, 4, // Side 1
        1, 2, 4, // Side 2
        2, 3, 4, // Side 3
        3, 0, 4  // Side 4
    };
}
bool Pyramid::InitObjectResources()
{
    TextureManager& textureManager = Engine::GetInstance().GetGraphicsEngine().GetTextureManager();

    SetVertexShaderPath("Triangle_VS.cso");
    SetPixelShaderPath("Triangle_PS.cso");

    if (!textureManager.LoadTexture("Pyramid", "lattice.png"))
    {
        return false;
    }
    SetTexture(textureManager.GetTexture("Pyramid"));

    return true;
}