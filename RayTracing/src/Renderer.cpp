#include "Renderer.h"

#include "Walnut/Random.h"

#define PI 3.14159265

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
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;


	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec4 color = RayGenPerPixel(x, y);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			// pass the color to image data
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::RayGenPerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	glm::vec3 color(0.0f);
	float multiplier = 1.0f;

	int bounces = 2;
	for (int i = 0; i < bounces; i++)
	{
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f)
		{
			glm::vec3 skyColor(0.1f, 0.1f, 0.1f);
			color += skyColor * multiplier;
			break;
		}
		// light
		glm::vec3 lightDir(1.0f, -1.0f, -1.0f);
		lightDir = glm::normalize(lightDir);
		float lightMultiplier = std::max(glm::dot(payload.HitNormal, -lightDir), 0.0f);

		// sphere
		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		glm::vec3 sphereColor = sphere.Albedo * lightMultiplier;

		// add into color
		color += sphereColor * multiplier;
		multiplier *= 0.7f;

		// move out a bit in case of dropping inside the sphere
		ray.Origin = payload.HitPosition + payload.HitNormal * 0.0001f;
		// to calculate reflectRay based on originRay and normal(must be normalized),
		// just filp the ray base at the normal direction in the distance of 2*|originRay|*cos¦È
		// reflectRay = originRay - 2 * (originRay * normal) * normal
		// ray.Direction = ray.Direction - 2 * glm::dot(ray.Direction, payload.HitNormal) * payload.HitNormal;
		ray.Direction = glm::reflect(ray.Direction, payload.HitNormal);
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
