#pragma once
#include <wrl/client.h>

#include "Object3D.h"

class Pyramid : public Object3D
{
public:
	Pyramid() = default;
	~Pyramid() = default;
	void CreateGeometry(std::vector<Vertex>& someVertices, std::vector<UINT>& someIndices) override;
	bool InitObjectResources() override;
};