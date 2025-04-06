//#define NUMBER_OF_LIGHTS_ALLOWED 8 
//#define MAX_ANIMATION_BONES 64 
//#define USE_LIGHTS
//#define USE_NOISE

#define PI 3.14159265358979323846f

Texture2D defaultTexture : register(t0);
Texture2D grassAlbedo : register(t1);
Texture2D rockAlbedo : register(t2);
Texture2D snowAlbedo : register(t3);
Texture2D grassNormalMap : register(t4);
Texture2D rockNormalMap : register(t5);
Texture2D snowNormalMap : register(t6);
Texture2D grassMaterialMap : register(t7);
Texture2D rockMaterialMap : register(t8);
Texture2D snowMaterialMap : register(t9);
Texture2D noiseTexture : register(t10);
TextureCube environmentTexture : register(t11);
Texture2D reflectionTexture : register(t12);
Texture2D fftWaveTexture : register(t13); // Precomputed FFT wave texture

SamplerState defaultSampler : register(s0);

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
    float depth : DEPTH;
};
struct PixelOutput
{
    float4 color : SV_TARGET0;
};

cbuffer FrameBuffer : register(b0)
{
    float4x4 worldToCamera;
    float4x4 cameraToProjection;
    float4x4 worldToClip;
	float4x4 worldToClipReflected;

    float3 cameraPosition;
    float time;

    float2 resolution;
    float waterHeight;
    float padding;
}
cbuffer ObjectBuffer : register(b1)
{
    float4x4 modelToWorld;
}
cbuffer LightBuffer : register(b2)
{
    float3 directionalLightDirection;
    float directionalLightIntensity;
    float4 directionalLightColor;
    float4 ambientColor1;
    float4 ambientColor2;
    float ambientIntensity1;
    float ambientIntensity2;
    float renderMode;
    float padding1;
}

float3 s_curve(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}
float3 GerstnerWave(float3 position, float amplitude, float wavelength, float speed, float direction, float time)
{
    float k = 2.0 * PI / wavelength;
    float c = sqrt(9.8 / k);
    float d = dot(float2(cos(direction), sin(direction)), position.xy);
    float phase = k * d - speed * time;

    float displacement = amplitude * sin(phase);
    float3 newPosition = position;
    newPosition.z += displacement;
    newPosition.xy += float2(cos(direction), sin(direction)) * amplitude * cos(phase);

    return newPosition;
}

float3 tonemap_s_gamut3_cine(float3 c)
{
    // based on Sony's s gamut3 cine
    float3x3 fromSrgb = float3x3(
        +0.6456794776, +0.2591145470, +0.0952059754,
        +0.0875299915, +0.7596995626, +0.1527704459,
        +0.0369574199, +0.1292809048, +0.8337616753);

    float3x3 toSrgb = float3x3(
        +1.6269474099, -0.5401385388, -0.0868088707,
        -0.1785155272, +1.4179409274, -0.2394254004,
        +0.0444361150, -0.1959199662, +1.2403560812);

    return mul(toSrgb, s_curve(mul(fromSrgb, c)));
}

int GetNumMips(TextureCube cubeTex)
{
	int iWidth = 0;
	int iheight = 0;
	int numMips = 0;
	cubeTex.GetDimensions(0, iWidth, iheight, numMips);
	return numMips;
}

float2x2 ComputeRotation(float aRotation)
{
    float c = cos(aRotation);
    float s = sin(aRotation);
    return float2x2(c, -s, s, c);
}

// This gets Log Depth from worldPosition
float GetLogDepth(float4 worldPosition)
{
    float4 cameraPos = mul(worldToCamera, worldPosition);
    float4 projectionPos = mul(cameraToProjection, cameraPos);
    return projectionPos.z / projectionPos.w;
}

//float GetLinDepth(float4 worldPosition)
//{
//    float logDepth = GetLogDepth(worldPosition);
//    return NearPlane / (FarPlane - logDepth * (FarPlane - NearPlane));
//}

//// Converts Log Depth to Lin Depth
//float LogDepthToLinDepth(float depth)
//{
//    return NearPlane / (FarPlane - depth * (FarPlane - NearPlane));
//}

//cbuffer FrameBuffer : register(b0)
//{
//	float4 Resolution; //myResolution.x = screen width, myResolution.y = screen height, myResolution.z = unset, myResolution.w = unset
//	float4 Timings; //myTimings.x = totaltime, myTimings.y = delta time, myTimings.z = unset, myTimings.w = unset
//}

//cbuffer CameraBuffer : register(b1)
//{
//	float4x4 WorldToCamera;
//	float4x4 CameraToProjection;
//	float4 CameraPosition;
//	float NearPlane;
//	float FarPlane;
//	float Unused0;
//	float Unused1;
//};

//cbuffer LightConstantBufferData : register(b2)
//{
//	struct PointLightData
//	{
//		float4 Position;
//		float4 Color;
//		float Range;
//		float3 garbage; // Padding, don't use arrays!
//	} myPointLights[NUMBER_OF_LIGHTS_ALLOWED];

//	uint NumberOfLights;
//	int NumEnvMapMipLevels;
//	float garbage0;
//	float garbage1;

//	float4 AmbientLightColorAndIntensity;
//	float4 DirectionalLightDirection;
//	float4 DirectionalLightColorAndIntensity;
//};

//cbuffer ShaderSettingsConstantBuffer : register(b3)
//{
//	float4 customShaderParameters;
//	float alphaTestThreshold;
//};

//cbuffer ObjectBuffer : register(b4)
//{
//	float4x4 ObjectToWorld;
//}

//cbuffer CustomShapeConstantBufferData : register(b4)
//{
//	float4x4 ModelToWorld;
//};

//cbuffer BoneBuffer : register(b5)
//{
//	float4x4 Bones[MAX_ANIMATION_BONES];
//};

//struct ModelVertexInput
//{
//	float4 position	    :	POSITION;
//	float4 vertexColor0	:	COLOR0;
//	float4 vertexColor1	:	COLOR1;
//	float4 vertexColor2	:	COLOR2;
//	float4 vertexColor3	:	COLOR3;
//	float2 texCoord0	:	TEXCOORD0;
//	float2 texCoord1	:	TEXCOORD1;
//	float2 texCoord2	:	TEXCOORD2;
//	float2 texCoord3	:	TEXCOORD3;
//	float3 normal		:	NORMAL;
//	float3 tangent		:	TANGENT;
//	float3 binormal	    :	BINORMAL;
//	float4 boneIndices  :   BONES;
//	float4 weights      :   WEIGHTS;
//};

//struct ModelVertexToPixel
//{
//	float4 position			:	SV_POSITION;
//	float4 worldPosition	:	POSITION;
//	float  depth		    :	DEPTH;
//	float4 vertexColor0		:	COLOR0;
//	float4 vertexColor1		:	COLOR1;
//	float4 vertexColor2		:	COLOR2;
//	float4 vertexColor3		:	COLOR3;
//	float2 texCoord0		:	TEXCOORD0;
//	float2 texCoord1		:	TEXCOORD1;
//	float2 texCoord2		:	TEXCOORD2;
//	float2 texCoord3		:	TEXCOORD3;
//	float3 normal			:	NORMAL;
//	float3 tangent			:	TANGENT;
//	float3 binormal			:	BINORMAL;
//};

//struct InstancedPixelInputType
//{
//	float4 position			:	SV_POSITION;
//	float4 worldPosition	:	POSITION;
//	float2 tex				:	TEXCOORD0;
//	float4 color			:	TEXCOORD2;
//	float3 normal			:	NORMAL;
//	uint instanceId			:	SV_InstanceID;
//};

//struct PixelOutput
//{
//	float4 color		:	SV_TARGET;
//};

//TextureCube environmentTexture : register(t0);

//Texture2D albedoTexture		: register(t1);
//Texture2D normalTexture		: register(t2);
//Texture2D materialTexture	: register(t3);
//Texture2D fxTexture			: register(t4);

//SamplerState defaultSampler : register(s0);

//// Get screen texture coordinates from world position?
//float2 GetScreenCoords(float4 worldPosition)
//{
//	float4 worldToView = mul(WorldToCamera, worldPosition);
//	float4 viewToProj = mul(CameraToProjection, worldToView);

//	float2 projectedTextureCoords;
//	projectedTextureCoords.x = viewToProj.x / viewToProj.w / 2.0f + 0.5f;
//	projectedTextureCoords.y = viewToProj.y / viewToProj.w / 2.0f + 0.5f;

//	return projectedTextureCoords;
//}