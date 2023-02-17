#include "Renderer.h"

#include "Walnut/Random.h"

#include <execution>

#define PI 3.14159265
#define MultiThread 1

namespace Utils
{
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		return 0x00000000 | a << 24 | b << 16 | g << 8 | r;
	}

	static glm::vec3 RandomUnitVec3()
	{
		while (true)
		{
			glm::vec3 v = Walnut::Random::Vec3(-1.0f, 1.0f);
			if (glm::dot(v,v) >= 1.0f)
				continue;
			return glm::normalize(v);
		}
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

	ResetAccumulationFrame();
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
	// shoot a ray
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	// light
	glm::vec3 lightDir(1.0f, -1.0f, -1.0f);
	lightDir = glm::normalize(lightDir);

	// output color
	glm::vec3 color(0.0f);
	float multiplier = 1.0f;

	// ray bounces
	int bounces = 20;
	for (int i = 0; i < bounces; i++)
	{
		// anti-aliasing
		ray.Direction += Walnut::Random::Vec3(-0.0006f, 0.0006f);

		// the hitPayload of every hit bounce
		HitPayload hitPayload = TraceRay(ray);
		if (hitPayload.ObjectIndex < 0)
		{
			glm::vec3 skyColor(0.8f, 0.9f, 1.0f);
			//skyColor = skyColor * ((float)y / m_FinalImage->GetHeight());
			color += skyColor * multiplier * (glm::dot(glm::normalize(ray.Direction), -lightDir) * 0.5f + 0.5f);
			break;
		}

		// surface light multiplier
		float lightMultiplier = std::max(glm::dot(hitPayload.HitNormal, -lightDir), -0.1f);

		// sphere & material
		const Sphere& sphere = m_ActiveScene->Spheres[hitPayload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];
		glm::vec3 sphereColor = material.Albedo * lightMultiplier;

		// add into color
		color += sphereColor * multiplier * std::sqrt(material.Opacity);

		// prepare for bouncing calculation
		glm::vec3 incident = glm::normalize(ray.Direction);
		glm::vec3 normal = glm::normalize(hitPayload.HitNormal + material.Roughness * 0.5f * Utils::RandomUnitVec3());

		if (hitPayload.HitRefracted)
		{
			// refract
			float IOR = 1.33f;
			if (hitPayload.IsFrontFace)
			{
				ray.Origin = hitPayload.HitPosition - hitPayload.HitNormal * 0.0001f;
				float refractionRatio = -1.0f / IOR;
				ray.Direction = glm::refract(incident, normal, refractionRatio);
			}
			else
			{
				ray.Origin = hitPayload.HitPosition + hitPayload.HitNormal * 0.0001f;
				float refractionRatio = IOR;
				ray.Direction = glm::refract(incident, -normal, refractionRatio);
			}
			multiplier *= material.Metallic * (1.0f - material.Opacity);
		}
		else
		{
			// reflect
			multiplier *= 0.35f * material.Metallic;
			ray.Origin = hitPayload.HitPosition + hitPayload.HitNormal * 0.0001f;
			ray.Direction = glm::reflect(ray.Direction, normal);
		}
	}

	return glm::vec4(color, 1);
}

HitPayload Renderer::TraceRay(const Ray& ray)
{
	// closet hit point
	int closestSphere = -1;
	float closestDistance = FLT_MAX;
	HitPayload closestHitPayload;

	// calculating if the ray intersects with the spheres
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		HitPayload hitPayload = Hit(ray, m_ActiveScene->Spheres[i]);

		// if intersects and showing in the front
		if (hitPayload.IsHit && hitPayload.HitDistance < closestDistance)
		{
			closestDistance = hitPayload.HitDistance;
			closestSphere = (int)i;
			closestHitPayload = hitPayload;
		}
	}

	// if hitting nothing
	if (closestSphere < 0)
	{
		return MissHit(closestHitPayload);
	}

	closestHitPayload.ObjectIndex = closestSphere;
	return ClosestHit(ray, closestHitPayload);
}

HitPayload Renderer::ClosestHit(const Ray& ray, HitPayload& hitPayload)
{
	const Sphere& closestSphere = m_ActiveScene->Spheres[hitPayload.ObjectIndex];

	hitPayload.HitPosition = ray.Origin + ray.Direction * hitPayload.HitDistance;
	hitPayload.HitNormal = glm::normalize(hitPayload.HitPosition - closestSphere.Origin);

	if (std::abs(Walnut::Random::Float()) > m_ActiveScene->Materials[closestSphere.MaterialIndex].Opacity)
	{
		hitPayload.HitRefracted = true;
	}
	else
	{
		hitPayload.HitRefracted = false;
	}

	return hitPayload;
}

HitPayload Renderer::MissHit(const HitPayload& hitPayload)
{
	return hitPayload;
}