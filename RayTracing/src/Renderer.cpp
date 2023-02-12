#include "Renderer.h"

#include "Walnut/Random.h"

#include <execution>

#define PI 3.14159265
#define MultiThread 1

namespace Utils
{
	struct QuadraticResult
	{
		float Discriminant;
		float T[2];
	};

	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		return 0x00000000 | a << 24 | b << 16 | g << 8 | r;
	}

	static QuadraticResult SolveQuadratic(const Ray& ray, const Sphere& sphere)
	{
		QuadraticResult result;

		// sphere
		glm::vec3 sphereOrigin = sphere.Origin;
		float sphereRadius = sphere.Radius;

		// calculating if the camera ray intersects with the sphere
		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * (glm::dot(ray.Origin, ray.Direction) - glm::dot(ray.Direction, sphereOrigin));
		float c = glm::dot(ray.Origin, ray.Origin) + glm::dot(sphereOrigin, sphereOrigin) - 2.0f * glm::dot(ray.Origin, sphereOrigin) - sphereRadius * sphereRadius;

		float discriminant = b * b - 4.0f * a * c;
		result.Discriminant = discriminant;
		
		if (discriminant > 0.0f)
		{
			// calculate the two hit points
			result.T[0] = (-b - glm::sqrt(discriminant)) / (2.0f * a);
			result.T[1] = (-b + glm::sqrt(discriminant)) / (2.0f * a);
		}

		return result;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

	m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);

	for (uint32_t i = 0; i < width; i++)
		m_ImageHorizontalIter[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_ImageVerticalIter[i] = i;

}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;

	if (m_AccumulationFrame == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetHeight() * m_FinalImage->GetWidth() * sizeof(glm::vec4));

	//std::thread::hardware_concurrency();

#if MultiThread

	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
		[this](uint32_t y)
		{
			std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
				[this, y](uint32_t x)
				{
					glm::vec4 color = RayGenPerPixel(x, y);
					m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;
					glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()] / (float)m_AccumulationFrame;
					accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));

					m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
				});
		});

#else

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec4 color = RayGenPerPixel(x, y);
			m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;
			glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()] / (float)m_AccumulationFrame;
			accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));

			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
		}
	}

#endif

	m_FinalImage->SetData(m_ImageData);

	if (m_Settings.Accumulate)
		m_AccumulationFrame++;
	else
		m_AccumulationFrame = 1;
}

glm::vec4 Renderer::RayGenPerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	glm::vec3 color(0.0f);
	float multiplier = 1.0f;

	int bounces = 5;
	for (int i = 0; i < bounces; i++)
	{
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f)
		{
			glm::vec3 skyColor(0.8f, 0.7f, 0.6f);
			//skyColor = skyColor * ((float)y / m_FinalImage->GetHeight());
			color += skyColor * multiplier;
			break;
		}
		// light
		glm::vec3 lightDir(1.0f, -1.0f, -1.0f);
		lightDir = glm::normalize(lightDir);
		float lightMultiplier = std::max(glm::dot(payload.HitNormal, -lightDir), 0.0f);

		// sphere & material
		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];
		glm::vec3 sphereColor = material.Albedo * lightMultiplier;

		// add into color
		color += sphereColor * multiplier;
		multiplier *= 0.35f * material.Metallic;

		// move out a bit in case of dropping inside the sphere
		ray.Origin = payload.HitPosition + payload.HitNormal * 0.0001f;
		// to calculate reflectRay based on originRay and normal(must be normalized),
		// just filp the ray base at the normal direction in the distance of 2*|originRay|*cos¦È
		// reflectRay = originRay - 2 * (originRay * normal) * normal
		// ray.Direction = ray.Direction - 2 * glm::dot(ray.Direction, payload.HitNormal) * payload.HitNormal;
		glm::vec3 reflectNormal = payload.HitNormal + material.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f);
		ray.Direction = glm::reflect(ray.Direction, reflectNormal);
	}

	return glm::vec4(color, 1);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	// closet hit point
	int closestSphere = -1;
	float closestDistance = FLT_MAX;

	// calculating if the ray intersects with the spheres
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		Utils::QuadraticResult result = Utils::SolveQuadratic(ray, m_ActiveScene->Spheres[i]);

		// if intersects and showing in the front
		if (result.Discriminant >= 0.0f && result.T[0] >= 0 && result.T[0] < closestDistance)
		{
			closestDistance = result.T[0];
			closestSphere = (int)i;
		}
	}

	if (closestSphere < 0)
	{
		return MissHit(ray);
	}

	return ClosestHit(ray, closestDistance, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float closestDistance, int closestObjectIndex)
{
	const Sphere& closestSphere = m_ActiveScene->Spheres[closestObjectIndex];

	Renderer::HitPayload payload;
	payload.HitDistance = closestDistance;
	payload.ObjectIndex = closestObjectIndex;
	payload.HitPosition = ray.Origin + ray.Direction * closestDistance;
	payload.HitNormal = glm::normalize(payload.HitPosition - closestSphere.Origin);
	return payload;
}

Renderer::HitPayload Renderer::MissHit(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}
