#include "Common.hlsli"

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    // Transform position
    float4 vertexObjectPos = input.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClip, vertexWorldPos);

    // Transform normal, tangent, and bitangent to world space
    float3x3 toWorldRotation = (float3x3)modelToWorld;
    float3 normalWorld = mul(toWorldRotation, input.normal);
    float3 tangentWorld = mul(toWorldRotation, input.tangent);
    float3 bitangentWorld = mul(toWorldRotation, input.bitangent);
    
    // Pass data to the pixel shader
    output.position = vertexClipPos;
    output.worldPosition = vertexWorldPos;
    output.color = input.color;
    output.uv = input.uv;

    output.normal = normalize(normalWorld);
    output.tangent = normalize(tangentWorld);
    output.bitangent = normalize(bitangentWorld);
    
    return output;
}