#pragma once
#include "Object3D.h"

class Plane : public Object3D
{
public:
	Plane() = default;
	~Plane() = default;
	void CreateGeometry(std::vector<Vertex>& someVertices, std::vector<UINT>& someIndices) override;
	void CalculateNormal(std::vector<Vertex>& someVertices);
	bool InitObjectResources() override;
};

