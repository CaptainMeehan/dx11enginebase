#pragma once
#include "Includes/Matrix4x4.h"

struct FrameBufferData
{
	CommonUtilities::Matrix4x4<float> worldToCamera;
	CommonUtilities::Matrix4x4<float> cameraToProjection;
	CommonUtilities::Matrix4x4<float> worldToClip;
	CommonUtilities::Matrix4x4<float> worldToClipReflected;

	CommonUtilities::Vector3<float> cameraPosition;
	float time;

	CommonUtilities::Vector2<float> resolution;
	float waterHeight;
	float padding;
};