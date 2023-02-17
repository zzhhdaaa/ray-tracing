#pragma once

#include <glm/glm.hpp>

struct BoundingBox
{
	BoundingBox(glm::vec3 a, glm::vec3 b) { MinPos = a; MaxPos = b; }
	glm::vec3 MinPos;
	glm::vec3 MaxPos;
};

class Geometry
{
public:
	int MaterialIndex = 0;
	virtual bool GetBoundingBox(BoundingBox& outputBox) const = 0;
};

class Sphere : public Geometry
{
public:
	Sphere() : Origin(glm::vec3(0.0f)), Radius(0.5f) {}
	Sphere(glm::vec3 origin, float radius) : Origin(origin), Radius(radius) {}
	glm::vec3 Origin;
	float Radius;

	virtual bool GetBoundingBox(BoundingBox& outputBox) const override;
};

class GeometryList : public Geometry
{
	virtual bool GetBoundingBox(BoundingBox& outputBox) const override;
};