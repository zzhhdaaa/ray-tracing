#pragma once

#include <glm/glm.hpp>

class Geometry
{
public:
	int MaterialIndex = 0;
};

class Sphere : public Geometry
{
public:
	int MaterialIndex = 0;

	glm::vec3 Origin{ 0.0f };
	float Radius = 0.5f;
};