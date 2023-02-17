#pragma once

#include <glm/glm.hpp>

#include "Geometry.h"

#include "Walnut/Random.h"

struct HitPayload
{
	// first-time calculation, done in RayMath::Hit
	bool IsHit = false;
	bool IsFrontFace = true;
	float HitDistance = -1.0f;

	// second-time calculation, done in Renderer::TraceRay
	int ObjectIndex = -1;

	// third-time calculation, done in RayMath::PayloadCompute
	bool HitRefracted = false;
	glm::vec3 HitPosition = glm::vec3(0, 0, 0);
	glm::vec3 HitNormal = glm::vec3(1, 0, 0);

	// uv
	glm::vec2 HitUV = glm::vec2(0, 0);
};

struct Ray
{
	Ray() : Origin(glm::vec3(0, 0, 0)), Direction(glm::vec3(1, 0, 0)) {}
	Ray(glm::vec3 origin, glm::vec3 direction) : Origin(origin), Direction(direction) {}
	glm::vec3 Origin;
	glm::vec3 Direction;
};

namespace RayMath
{
#define PI 3.14159265
	bool Hit(const Ray& ray, const BoundingBox& box);
	HitPayload Hit(const Ray& ray, const Geometry& geometry);
	HitPayload Hit(const Ray& ray, const Sphere& sphere);
	HitPayload PayloadCompute(HitPayload& payload, const Ray& ray, const Sphere& sphere, const float& opacity);
}