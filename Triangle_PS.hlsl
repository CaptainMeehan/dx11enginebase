Texture2D grassTexture : register(t0);
Texture2D rockTexture : register(t1);
Texture2D snowTexture : register(t2);
Texture2D grassNormalMap : register(t3);
Texture2D rockNormalMap : register(t4);
Texture2D snowNormalMap : register(t5);
SamplerState aSampler : register(s0);

cbuffer LightBuffer : register(b2)
{
    float3 directionalLightDirection;
    float directionalLightIntensity;
    float4 directionalLightColor;
    float4 ambientColor1;
    float4 ambientColor2;
    float ambientIntensity1;
    float ambientIntensity2;
    float padding1;
    float padding2;
}

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

struct PixelOutput
{
    float4 color : SV_TARGET0;
};

PixelOutput main(PixelInputType input)
{
    PixelOutput result;

    // Sample normal maps
    float3 grassNormalSample = grassNormalMap.Sample(aSampler, input.uv).rgb * 2.0 - 1.0;
    float3 rockNormalSample = rockNormalMap.Sample(aSampler, input.uv).rgb * 2.0 - 1.0;
    float3 snowNormalSample = snowNormalMap.Sample(aSampler, input.uv).rgb * 2.0 - 1.0;

    // Construct TBN matrix
    float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);

    // Transform normal map samples to world space
    float3 grassNormal = normalize(mul(TBN, grassNormalSample));
    float3 rockNormal = normalize(mul(TBN, rockNormalSample));
    float3 snowNormal = normalize(mul(TBN, snowNormalSample));

    // Compute slope blend factor
    float slopeBlend = saturate(1.0f - abs(input.normal.y) * 0.8f); // Steeper slopes have higher values, blend slightly less steep slopes

    // Compute height blend factor
    float heightBlend = smoothstep(75.0f, 100.0f, input.worldPosition.y); // Adjust to match your terrain height range

    // Sample textures
    float4 grassColor = grassTexture.Sample(aSampler, input.uv);
    float4 rockColor = rockTexture.Sample(aSampler, input.uv);
    float4 snowColor = snowTexture.Sample(aSampler, input.uv);

    // Blend snow based on height first
    float3 colorAfterHeight = lerp(rockColor.rgb, snowColor.rgb, heightBlend);
    float3 blendedColor = lerp(grassColor.rgb, colorAfterHeight, slopeBlend);

    // Blend normals in the same order
    float3 normalAfterHeight = normalize(lerp(rockNormal, snowNormal, heightBlend));
    float3 blendedNormal = normalize(lerp(grassNormal, normalAfterHeight, slopeBlend));

    // Compute directional light contribution
    float3 lightDir = normalize(-directionalLightDirection);
    float diffIntensity = max(dot(blendedNormal, lightDir), 0.0) * directionalLightIntensity;
    float4 directionalLight = diffIntensity * directionalLightColor;

    // Compute ambient light contributions
    float4 ambientLight1 = ambientColor1 * ambientIntensity1;
    float4 ambientLight2 = ambientColor2 * ambientIntensity2;

    // Combine lighting
    float4 finalLighting = directionalLight + ambientLight1 + ambientLight2;

    // Combine lighting with blended color
    result.color = float4(blendedColor, 1.0) * finalLighting;

    return result;
}
