#include "Terrain.h"
#include <vector>
#include <cmath>
#include <fstream>

#include "Includes/PCG/PerlinNoise.hpp" // Include Perlin noise header

#include "Engine.h"
#include "GraphicsEngine.h"

float GenerateHeight(float x, float z, const siv::PerlinNoise& perlin, float baseScale, float heightScale, int octaves, float persistence, float lacunarity)
{
    float frequency = baseScale;
    float amplitude = heightScale;
    float noiseHeight = 0.0f;

    for (int i = 0; i < octaves; ++i)
    {
        noiseHeight += static_cast<float>(perlin.octave2D_01(x * frequency, z * frequency, 1)) * amplitude;
        frequency *= lacunarity;
        amplitude *= persistence;
    }

    return noiseHeight/* - (heightScale / 2.0f)*/; // Center the height
}
void ApplyRegionModifiers(std::vector<Vertex>& vertices, int gridSize, const siv::PerlinNoise& perlin, float regionScale, int numLakes, int numMountains, int numPlains)
{
    float regionSize = static_cast<float>(gridSize) * regionScale;

    for (int z = 0; z <= gridSize; ++z)
    {
        for (int x = 0; x <= gridSize; ++x)
        {
            int index = z * (gridSize + 1) + x;
            Vertex& vertex = vertices[index];

            float heightAdjustment = 0.0f;

            // Lakes
            for (int i = 0; i < numLakes; ++i)
            {
                float lakeX = perlin.noise2D_01(i * 10.0f, 0.0f) * gridSize;
                float lakeZ = perlin.noise2D_01(0.0f, i * 10.0f) * gridSize;
                float distance = sqrt(pow(x - lakeX, 2) + pow(z - lakeZ, 2));
                heightAdjustment -= 10.0f * exp(-distance / (regionSize * 0.25f)); // Smooth falloff
            }

            // Mountains
            for (int i = 0; i < numMountains; ++i)
            {
                float mountainX = perlin.noise2D_01(i * 20.0f, 0.0f) * gridSize;
                float mountainZ = perlin.noise2D_01(0.0f, i * 20.0f) * gridSize;
                float distance = sqrt(pow(x - mountainX, 2) + pow(z - mountainZ, 2));
                heightAdjustment += 50.0f * exp(-distance / (regionSize * 0.2f)); // Taller, sharper falloff
            }

            // Plains
            for (int i = 0; i < numPlains; ++i)
            {
                float plainX = perlin.noise2D_01(i * 30.0f, 0.0f) * gridSize;
                float plainZ = perlin.noise2D_01(0.0f, i * 30.0f) * gridSize;
                float distance = sqrt(pow(x - plainX, 2) + pow(z - plainZ, 2));
                heightAdjustment = std::lerp(vertex.y, 0.0f, exp(-distance / (regionSize * 0.3f))); // Flatten smoothly
            }

            vertex.y += heightAdjustment;
        }
    }
}
void ComputeTangentsAndBitangents(std::vector<Vertex>& vertices, const std::vector<UINT>& indices) {
    for (size_t i = 0; i < indices.size(); i += 3) {
        UINT i0 = indices[i];
        UINT i1 = indices[i + 1];
        UINT i2 = indices[i + 2];

        Vertex& v0 = vertices[i0];
        Vertex& v1 = vertices[i1];
        Vertex& v2 = vertices[i2];

        // Position vectors
        CommonUtilities::Vector3<float> pos0(v0.x, v0.y, v0.z);
        CommonUtilities::Vector3<float> pos1(v1.x, v1.y, v1.z);
        CommonUtilities::Vector3<float> pos2(v2.x, v2.y, v2.z);

        // Texture coordinates
        CommonUtilities::Vector2<float> uv0(v0.u, v0.v);
        CommonUtilities::Vector2<float> uv1(v1.u, v1.v);
        CommonUtilities::Vector2<float> uv2(v2.u, v2.v);

        // Edges of the triangle
        auto edge1 = pos1 - pos0;
        auto edge2 = pos2 - pos0;

        auto deltaUV1 = uv1 - uv0;
        auto deltaUV2 = uv2 - uv0;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        // Compute tangent and bitangent
        CommonUtilities::Vector3<float> tangent, bitangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        // Orthogonalize tangent to the normal
        CommonUtilities::Vector3<float> normal(v0.nx, v0.ny, v0.nz);
        tangent = tangent - (tangent.Dot(normal) * normal);
        tangent.Normalize();

        // Recalculate the bitangent
        bitangent = normal.Cross(tangent);
        bitangent.Normalize();

        // Add tangent and bitangent to vertices
        v0.tx += tangent.x; v0.ty += tangent.y; v0.tz += tangent.z;
        v1.tx += tangent.x; v1.ty += tangent.y; v1.tz += tangent.z;
        v2.tx += tangent.x; v2.ty += tangent.y; v2.tz += tangent.z;

        v0.bx += bitangent.x; v0.by += bitangent.y; v0.bz += bitangent.z;
        v1.bx += bitangent.x; v1.by += bitangent.y; v1.bz += bitangent.z;
        v2.bx += bitangent.x; v2.by += bitangent.y; v2.bz += bitangent.z;
    }

    // Normalize tangents and bitangents
    for (auto& v : vertices) {
        CommonUtilities::Vector3<float> tangent(v.tx, v.ty, v.tz);
        CommonUtilities::Vector3<float> bitangent(v.bx, v.by, v.bz);

        tangent.Normalize();
        bitangent.Normalize();

        v.tx = tangent.x; v.ty = tangent.y; v.tz = tangent.z;
        v.bx = bitangent.x; v.by = bitangent.y; v.bz = bitangent.z;
    }
}

bool Terrain::Initialize(ID3D11Device* aDevice)
{
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
    CreateGeometry(vertices, indices);
    InitObjectResources();
    if (!CreateBuffers(aDevice, vertices, indices))
    {
        return false;
    }
    if (!LoadTestShaders(aDevice))
    {
        return false;
    }
    return LoadShadersAndCreateInputLayout(aDevice);
}
void Terrain::Render(ID3D11DeviceContext* aContext)
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
void Terrain::RenderDiffuse(ID3D11DeviceContext* aContext)
{
    aContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    aContext->IASetInputLayout(myInputLayout.Get());
    unsigned int stride = sizeof(Vertex);
    unsigned int offset = 0;
    aContext->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
    aContext->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    aContext->VSSetShader(myVertexShader.Get(), nullptr, 0);
    aContext->PSSetShader(myDiffusePixelShader.Get(), nullptr, 0);
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
void Terrain::RenderSpecular(ID3D11DeviceContext* aContext)
{
    aContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    aContext->IASetInputLayout(myInputLayout.Get());
    unsigned int stride = sizeof(Vertex);
    unsigned int offset = 0;
    aContext->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
    aContext->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    aContext->VSSetShader(myVertexShader.Get(), nullptr, 0);
    aContext->PSSetShader(mySpecularPixelShader.Get(), nullptr, 0);
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

void Terrain::CreateGeometry(std::vector<Vertex>& someVertices, std::vector<UINT>& someIndices)
{
    myGridSize = 256;
    myTileSize = 1; // Resolution

    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{ seed };

    int numVertices = myGridSize + 1; // One extra row/column for vertices
    someVertices.resize(numVertices * numVertices);
    someIndices.reserve(myGridSize * myGridSize * 6); // Two triangles per tile

    float baseScale = 0.02f;      // Adjust for larger-scale features
    float heightScale = 50.0f;    // Overall height range
    int octaves = 6;              // More layers for detail
    float persistence = 0.5f;     // Amplitude decay
    float lacunarity = 2.0f;      // Frequency growth

    for (int z = 0; z <= myGridSize; ++z)
    {
        for (int x = 0; x <= myGridSize; ++x)
        {
            float fx = static_cast<float>(x) * static_cast<float>(myTileSize);
            float fz = static_cast<float>(z) * static_cast<float>(myTileSize);
            float height = GenerateHeight(fx, fz, perlin, baseScale, heightScale, octaves, persistence, lacunarity);

            Vertex vertex = {};
            vertex.x = fx;
            vertex.y = height;
            vertex.z = -fz;
            vertex.w = 1.0f;

            // Set color based on height
            vertex.r = 0.0f;
            vertex.g = vertex.y > 0 ? 1.0f : 0.0f;
            vertex.b = vertex.y < 0 ? 1.0f : 0.0f;
            vertex.a = 1.0f;

            vertex.nx = 0.0f; // Placeholder normal
            vertex.ny = 1.0f;
            vertex.nz = 0.0f;
            vertex.u = static_cast<float>(x) / static_cast<float>(myGridSize); // Texture coordinate
            vertex.v = static_cast<float>(z) / static_cast<float>(myGridSize);
            someVertices[z * numVertices + x] = vertex;
        }
    }

    ApplyRegionModifiers(someVertices, myGridSize, perlin, 0.5f, 2, 2, 2);

    for (int z = 0; z < myGridSize; ++z)
    {
        for (int x = 0; x < myGridSize; ++x)
        {
            int topLeft = z * numVertices + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * numVertices + x;
            int bottomRight = bottomLeft + 1;

            // First triangle
            someIndices.push_back(topLeft);
            someIndices.push_back(bottomLeft);
            someIndices.push_back(topRight);

            // Second triangle
            someIndices.push_back(topRight);
            someIndices.push_back(bottomLeft);
            someIndices.push_back(bottomRight);
        }
    }

    // Accumulate normals
    std::vector<CommonUtilities::Vector3<float>> accumulatedNormals(someVertices.size(), CommonUtilities::Vector3<float>(0.0f, 0.0f, 0.0f));
    std::vector<int> adjacentFaces(someVertices.size(), 0);

    for (size_t i = 0; i < someIndices.size(); i += 3)
    {
        // Indices of the triangle
        UINT i0 = someIndices[i];
        UINT i1 = someIndices[i + 1];
        UINT i2 = someIndices[i + 2];
        // Get the vertices
        Vertex& v0 = someVertices[i0];
        Vertex& v1 = someVertices[i1];
        Vertex& v2 = someVertices[i2];

        // Compute edges
        CommonUtilities::Vector3<float> pos0(v0.x, v0.y, v0.z);
        CommonUtilities::Vector3<float> pos1(v1.x, v1.y, v1.z);
        CommonUtilities::Vector3<float> pos2(v2.x, v2.y, v2.z);

        auto edge1 = pos1 - pos0;
        auto edge2 = pos2 - pos0;

        // Calculate the face normal
        auto faceNormal = edge1.Cross(edge2).GetNormalized();

        // Accumulate the normal for each vertex
        accumulatedNormals[i0] += faceNormal;
        accumulatedNormals[i1] += faceNormal;
        accumulatedNormals[i2] += faceNormal;

        // Track the number of faces affecting each vertex
        adjacentFaces[i0]++;
        adjacentFaces[i1]++;
        adjacentFaces[i2]++;
    }

    // Normalize accumulated normals and assign them to the vertices
    for (size_t i = 0; i < someVertices.size(); ++i)
    {
        if (adjacentFaces[i] > 0)
        {
            accumulatedNormals[i] /= static_cast<float>(adjacentFaces[i]);
            accumulatedNormals[i].Normalize();
        }

        someVertices[i].nx = accumulatedNormals[i].x;
        someVertices[i].ny = accumulatedNormals[i].y;
        someVertices[i].nz = accumulatedNormals[i].z;
    }

    // Compute tangents and bitangents for lighting and texture mapping
    ComputeTangentsAndBitangents(someVertices, someIndices);
}
bool Terrain::InitObjectResources()
{
    SetVertexShaderPath("Terrain_VS.cso");
    SetPixelShaderPath("Terrain_PS.cso");

    TextureManager& textureManager = Engine::GetInstance().GetGraphicsEngine().GetTextureManager();
    SetTexture(textureManager.GetTexture("Default"));

    return true;
}
bool Terrain::LoadTestShaders(ID3D11Device* aDevice)
{
    std::string solutionDir = SOLUTION_DIR;
    std::string shaderDir = "TGP/Shaders/";

    std::string diffusePixelShaderPath = solutionDir + shaderDir + "Terrain_Diffuse_PS.cso";
    std::string specularPixelShaderPath = solutionDir + shaderDir + "Terrain_Specular_PS.cso";

    std::ifstream psFile;
    psFile.open(diffusePixelShaderPath, std::ios::binary);
    if (!psFile.is_open())
    {
        return false;
    }
    std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
    psFile.close();

    HRESULT hr = aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myDiffusePixelShader);
    if (FAILED(hr))
    {
    	return false;
    }

    psFile.open(specularPixelShaderPath, std::ios::binary);
    if (!psFile.is_open())
    {
        return false;
    }
    psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
    psFile.close();

    hr = aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &mySpecularPixelShader);
    if (FAILED(hr))
    {
    	return false;
    }

    return true;
}