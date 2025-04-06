cbuffer FrameBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
    float time;
    float padding1;
    float padding2;
    float padding3;
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 modelToWorld;
}

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    // Transform position
    float4 vertexObjectPos = input.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);

    // Transform normal, tangent, and bitangent to world space
    float3 normalWorld = mul((float3x3) modelToWorld, input.normal);
    float3 tangentWorld = mul((float3x3) modelToWorld, input.tangent);
    float3 bitangentWorld = mul((float3x3) modelToWorld, input.bitangent);

    // Pass data to the pixel shader
    output.position = vertexClipPos;
    output.worldPosition = vertexWorldPos;
    output.color = input.color;
    output.uv = input.uv;
    output.normal = normalize(normalWorld); // Normalize the normal vector
    output.tangent = normalize(tangentWorld); // Normalize the tangent vector
    output.bitangent = normalize(bitangentWorld); // Normalize the bitangent vector

    return output;
}