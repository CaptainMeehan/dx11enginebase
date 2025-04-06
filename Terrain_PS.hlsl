#include "Common.hlsli"
#include "PBRFunctions.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    
    float waterHeightOffset = 0.099f;

    if (input.worldPosition.y < waterHeight - waterHeightOffset)
    {
        clip(-1);
    }

    float2 scaledUV = input.uv;
	
    float3 toEye = normalize(cameraPosition- input.worldPosition.xyz);
    float4 albedoGrass = grassAlbedo.Sample(defaultSampler, scaledUV).rgba;
    float4 albedoRock = rockAlbedo.Sample(defaultSampler, scaledUV).rgba;
    float4 albedoSnow = snowAlbedo.Sample(defaultSampler, scaledUV).rgba;
	
    float3 normalGrass = grassNormalMap.Sample(defaultSampler, scaledUV).xyy;
    float3 normalRock = rockNormalMap.Sample(defaultSampler, scaledUV).xyy;
    float3 normalSnow = snowNormalMap.Sample(defaultSampler, scaledUV).xyy;

    normalGrass.xy = 2.0f * normalGrass.xy - 1.0f;
    normalGrass.z = sqrt(1 - saturate(normalGrass.x * normalGrass.x + normalGrass.y * normalGrass.y));
    normalGrass = normalize(normalGrass);
    normalRock.xy = 2.0f * normalRock.xy - 1.0f;
    normalRock.z = sqrt(1 - saturate(normalRock.x * normalRock.x + normalRock.y * normalRock.y));
    normalRock = normalize(normalRock);
    normalSnow.xy = 2.0f * normalSnow.xy - 1.0f;
    normalSnow.z = sqrt(1 - saturate(normalSnow.x * normalSnow.x + normalSnow.y * normalSnow.y));
    normalSnow = normalize(normalSnow);

    float3x3 TBN = float3x3(
		normalize(input.tangent.xyz),
		normalize(-input.bitangent.xyz),
		normalize(input.normal.xyz)
		);
    
    TBN = transpose(TBN);
    float3 grassNormal = normalize(mul(TBN, normalGrass));
    float3 rockNormal = normalize(mul(TBN, normalRock));
    float3 snowNormal = normalize(mul(TBN, normalSnow));
    
    float3 materialGrass = grassMaterialMap.Sample(defaultSampler, scaledUV).rgb;
    float3 materialRock = rockMaterialMap.Sample(defaultSampler, scaledUV).rgb;
    float3 materialSnow = snowMaterialMap.Sample(defaultSampler, scaledUV).rgb;
    
    // Compute slope factor (steepness)
    float slopeFactor = saturate(1.0f - abs(input.normal.y)); // Steeper slopes have higher values

    // Compute height factors for snow and grass
    float heightFactor = input.worldPosition.y;
    float snowStart = 90.0f;
    float snowStartEnd = 110.0f;

    // Smooth transitions
    float snowHeightBlend = smoothstep(snowStart, snowStartEnd, heightFactor);
    
    // Add noise for uneven blending
    float2 noiseUV = input.worldPosition.xz * 0.1; // Scale noise UV
    float noiseValue = noiseTexture.Sample(defaultSampler, noiseUV).r * 2.0 - 1.0; // Center noise between -1 and 1

    // Perturb snow and grass blend
    float perturbation = noiseValue * 0.1; // Scale perturbation
    snowHeightBlend = saturate(snowHeightBlend + perturbation);

    // Compute individual weights
    float grassWeight = (1.0f - slopeFactor) * (1.0f - snowHeightBlend); // Grass on flat areas, no snow
    float rockWeight = slopeFactor * (1.0f - snowHeightBlend); // Rock on steep slopes, no snow
    float snowWeight = snowHeightBlend; // Snow based on height

    // Normalize weights
    float totalWeight = grassWeight + rockWeight + snowWeight;
    grassWeight /= totalWeight;
    rockWeight /= totalWeight;
    snowWeight /= totalWeight;
    
    // Extract material properties
    float grassAO = materialGrass.r;
    float rockAO = materialRock.r;
    float snowAO = materialSnow.r;

    float grassRoughness = materialGrass.g;
    float rockRoughness = materialRock.g;
    float snowRoughness = materialSnow.g;

    float grassMetallic = materialGrass.b;
    float rockMetallic = materialRock.b;
    float snowMetallic = materialSnow.b;
    
    // Blend materials
    float3 blendedColor = albedoGrass.rgb * grassWeight +
                          albedoRock.rgb * rockWeight +
                          albedoSnow.rgb * snowWeight;

    float blendedA = albedoGrass.a * grassWeight +
                          albedoRock.a * rockWeight +
                          albedoSnow.a * snowWeight;

    float blendedRoughness = grassRoughness * grassWeight +
                             rockRoughness * rockWeight +
                             snowRoughness * snowWeight;

    float blendedMetallic = grassMetallic * grassWeight +
                            rockMetallic * rockWeight +
                            snowMetallic * snowWeight;

    float blendedAO = grassAO * grassWeight +
                      rockAO * rockWeight +
                      snowAO * snowWeight;

    float3 blendedNormal = normalize(grassNormal * grassWeight +
                                     rockNormal * rockWeight +
                                     snowNormal * snowWeight);
    
    float3 specularColor = lerp((float3) 0.04f, blendedColor, blendedMetallic);
    float3 diffuseColor = lerp((float3) 0.00f, blendedColor, 1 - blendedMetallic);

    float3 ambiance = EvaluateAmbiance(
		environmentTexture, blendedNormal, input.normal.xyz,
		toEye, blendedRoughness,
		blendedAO, diffuseColor, specularColor
	);

    float3 directionalLightColorAndIntensity = directionalLightColor * directionalLightIntensity;

    float3 directionalLight = EvaluateDirectionalLight(
		diffuseColor, specularColor, blendedNormal, blendedRoughness,
		directionalLightColorAndIntensity.xyz, directionalLightDirection.xyz, toEye.xyz
	);
	
    float3 radiance = ambiance + directionalLight + blendedColor;
    
    result.color.rgb = tonemap_s_gamut3_cine((float3) radiance);
    result.color.a = blendedA;
    return result;
}
