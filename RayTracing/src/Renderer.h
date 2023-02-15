#pragma once

#include "Walnut/Image.h"

#include "Scene.h"
#include "Camera.h"
#include "Ray.h"

#include <memory>
#include <glm/glm.hpp>

class Renderer
{
public:
	struct Settings
	{
		bool Accumulate = true;
	};
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	void ResetAccumulationFrame() { m_AccumulationFrame = 1; }
	Settings& GetSettings() { return m_Settings; }
private:
	struct QuadraticResult
	{
		float Discriminant;
		float T[2];
		bool IsFrontFace;
	};

	struct HitPayload
	{
		float HitDistance = 0.0f;
		glm::vec3 HitPosition;
		glm::vec3 HitNormal;

		int ObjectIndex = 0;
		bool HitRefracted = false;
		bool IsFrontFace = true;
	};

	glm::vec4 RayGenPerPixel(uint32_t x, uint32_t y);

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, const QuadraticResult& closestResult, int closestObjectIndex);
	HitPayload MissHit(const Ray& ray);

	QuadraticResult SolveQuadratic(const Ray& ray, const Sphere& sphere);
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	Settings m_Settings;

	std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_AccumulationFrame = 1;
};