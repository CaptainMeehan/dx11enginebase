#include "Common.hlsli"
#include "PBRFunctions.hlsli"

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    float3 viewDirection = normalize(cameraPosition - input.position.xyz);

    // Apply Gerstner wave function
    float amplitude = 20.1f;
    float wavelength = 50.0f;
    float speed = 5.0f;
    float direction = 90.0f; // Direction in radians

    float3 wavePosition = GerstnerWave(input.position.xyz, amplitude, wavelength, speed, direction, time);

    float distanceFromWater = input.worldPosition.y - waterHeight;
    float waterHeightOffset = 0.099f;
    float waterBlendFactor = saturate((distanceFromWater + waterHeightOffset) / (2.0f * waterHeightOffset));

    float3 reflectionBaseColor = reflectionTexture.Sample(defaultSampler, wavePosition.xy / resolution).rgb;
    
    float3 fresnel = Fresnel_Schlick(float3(0.25f, 0.25f, 0.25f), float3(0.0f, 1.0f, 0.0f), viewDirection);

    // Blend reflection color with terrain color based on waterBlendFactor
    float3 terrainColor = reflectionBaseColor; // Replace with actual terrain color if available
    float3 blendedColor = lerp(terrainColor * fresnel, reflectionBaseColor * fresnel, waterBlendFactor);
    
    result.color.rgb = blendedColor;
    result.color.a = 1.0f; // Fully opaque
    
    // Define the clear color
    float3 clearColor = float3(0.68f, 0.85f, 0.90f);

    // Check if the resulting color is the clear color
    if (all(result.color.rgb == clearColor))
    {
        // Sample the surrounding pixels
        float2 texelSize = 1.0f / resolution;
        float3 color1 = reflectionTexture.Sample(defaultSampler, wavePosition.xy / resolution + float2(-texelSize.x, 0)).rgb;
        float3 color2 = reflectionTexture.Sample(defaultSampler, wavePosition.xy / resolution + float2(texelSize.x, 0)).rgb;
        float3 color3 = reflectionTexture.Sample(defaultSampler, wavePosition.xy / resolution + float2(0, -texelSize.y)).rgb;
        float3 color4 = reflectionTexture.Sample(defaultSampler, wavePosition.xy / resolution + float2(0, texelSize.y)).rgb;

        // Count the number of non-clear colors
        int nonClearCount = 0;
        if (length(color1) > 0.0f) nonClearCount++;
        if (length(color2) > 0.0f) nonClearCount++;
        if (length(color3) > 0.0f) nonClearCount++;
        if (length(color4) > 0.0f) nonClearCount++;

        // Blend the colors if more than 2 surrounding pixels have a color
        if (nonClearCount > 2)
        {
            result.color.rgb = (color1 + color2 + color3 + color4) / 4.0f;
            result.color.a = 1.0f; // Set alpha to fully opaque
        }
    }

    return result;
}
