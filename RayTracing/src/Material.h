#pragma once

#include <glm/glm.hpp>

#include "Texture.h"

struct Material
{
	glm::vec3 Albedo{ 1.0f };
	//SolidColor Texture = SolidColor(glm::vec3(1.0f));
	float Roughness = 1.0f;
	float Metallic = 1.0f;
	float Opacity = 1.0f;

	std::shared_ptr<Texture> Texture;
};