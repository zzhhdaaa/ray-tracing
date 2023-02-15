#pragma once

#include <glm/glm.hpp>

struct Sphere
{
	glm::vec3 Origin{ 0.0f };
	float Radius = 0.5f;

	int MaterialIndex = 0;
};