#pragma once

#include <glm/glm.hpp>

struct Material
{
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.0f;
	float Metallic = 1.0f;
	float Opacity = 1.0f;
};