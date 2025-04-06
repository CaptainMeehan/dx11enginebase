#pragma once
#include "Includes/MeehanVector3.hpp"
#include "Includes/MeehanVector4.hpp"

struct LightBuffer
{
    CommonUtilities::Vector3<float> directionalLightDirection;
    float directionalLightIntensity;
    CommonUtilities::Vector4<float> directionalLightColor;
    CommonUtilities::Vector4<float> ambientColor1;
    CommonUtilities::Vector4<float> ambientColor2;
    float ambientIntensity1;
    float ambientIntensity2;
    float renderMode;
    float padding1;
};
