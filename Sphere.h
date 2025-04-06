#pragma once
#include <directxmath.h>
#include "Object3D.h"
#include "Vertex.h"
#include <d3d11.h>

class Sphere : public Object3D
{
public:
	Sphere() = default;
	~Sphere() = default;

	void CreateGeometry(std::vector<Vertex>& someVertices, std::vector<UINT>& someIndices) override;
	bool InitObjectResources() override;
};
