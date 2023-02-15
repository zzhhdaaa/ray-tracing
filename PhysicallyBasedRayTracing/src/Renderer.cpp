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

	static glm::vec3 RefractedDirection(const glm::vec3& inDir, const glm::vec3& hitNor, float eta)
	{
		float cosThetaI = glm::dot(hitNor, inDir);
		float sin2ThetaI = std::max(0.0f, (float)(1 - cosThetaI * cosThetaI));
		float sin2ThetaT = eta * eta * sin2ThetaI;

		//if (sin2ThetaT >= 1) return inDir;
		float cosThetaT = std::sqrt(1 - sin2ThetaT);
		glm::vec3 outDir = eta * (-inDir) + (eta * cosThetaI - cosThetaT) * hitNor;
		return outDir;
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

		// the payload of every hit bounce
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f)
		{
			glm::vec3 skyColor(0.8f, 0.9f, 1.0f);
			//skyColor = skyColor * ((float)y / m_FinalImage->GetHeight());
			color += skyColor * multiplier * (glm::dot(glm::normalize(ray.Direction), -lightDir) * 0.5f + 0.5f);
			break;
		}

		// surface light multiplier
		float lightMultiplier = std::max(glm::dot(payload.HitNormal, -lightDir), -0.1f);

		// sphere & material
		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];
		glm::vec3 sphereColor = material.Albedo * lightMultiplier;

		// add into color
		color += sphereColor * multiplier * std::sqrt(material.Opacity);

		glm::vec3 reflectNormal = glm::normalize(payload.HitNormal + material.Roughness * 0.5f * Utils::RandomUnitVec3());
		glm::vec3 inDir = glm::normalize(ray.Direction);

		if (payload.HitRefracted)
		{
			float ir = 1.33f;
			if (payload.IsFrontFace)
			{
				// move in a bit in case of dropping inside the sphere
				ray.Origin = payload.HitPosition - payload.HitNormal * 0.0001f;
				float refractionRatio = -1.0f / ir;
				//ray.Direction = Utils::RefractedDirection(ray.Direction, reflectNormal, refractionRatio);
				ray.Direction = glm::refract(inDir, reflectNormal, refractionRatio);
			}
			else
			{
				// move in a bit out case of dropping inside the sphere
				ray.Origin = payload.HitPosition + payload.HitNormal * 0.0001f;
				float refractionRatio = ir;
				//ray.Direction = Utils::RefractedDirection(ray.Direction, -reflectNormal, refractionRatio);
				ray.Direction = glm::refract(inDir, -reflectNormal, refractionRatio);
			}
			multiplier *= material.Metallic * (1.0f - material.Opacity);
		}
		else
		{
			multiplier *= 0.35f * material.Metallic;
			// move out a bit in case of dropping inside the sphere
			ray.Origin = payload.HitPosition + payload.HitNormal * 0.0001f;
			// to calculate reflectRay based on originRay and normal(must be normalized),
			// just filp the ray base at the normal direction in the distance of 2*|originRay|*cos¦È
			// reflectRay = originRay - 2 * (originRay * normal) * normal
			// ray.Direction = ray.Direction - 2 * glm::dot(ray.Direction, payload.HitNormal) * payload.HitNormal;
			ray.Direction = glm::reflect(ray.Direction, reflectNormal);
		}
	}

	return glm::vec4(color, 1);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	// closet hit point
	int closestSphere = -1;
	float closestDistance = FLT_MAX;
	bool closestIsFrontFace = true;
	Renderer::QuadraticResult closestResult;

	// calculating if the ray intersects with the spheres
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		Renderer::QuadraticResult result = Renderer::SolveQuadratic(ray, m_ActiveScene->Spheres[i]);

		// if intersects and showing in the front
		if (result.Discriminant >= 0.0f && result.T[0] >= 0 && result.T[0] < closestDistance)
		{
			closestDistance = result.T[0];
			closestSphere = (int)i;
			closestIsFrontFace = result.IsFrontFace;
			closestResult = result;
		}
	}

	if (closestSphere < 0)
	{
		return MissHit(ray);
	}

	return ClosestHit(ray, closestResult, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, const QuadraticResult& closestResult, int closestObjectIndex)
{
	const Sphere& closestSphere = m_ActiveScene->Spheres[closestObjectIndex];

	Renderer::HitPayload payload;
	payload.HitDistance = closestResult.T[0];
	payload.HitPosition = ray.Origin + ray.Direction * closestResult.T[0];
	payload.HitNormal = glm::normalize(payload.HitPosition - closestSphere.Origin);
	payload.IsFrontFace = closestResult.IsFrontFace;
	payload.ObjectIndex = closestObjectIndex;

	if (std::abs(Walnut::Random::Float()) > m_ActiveScene->Materials[closestSphere.MaterialIndex].Opacity)
	{
		payload.HitRefracted = true;
	}
	else
	{
		payload.HitRefracted = false;
	}

	return payload;
}

Renderer::HitPayload Renderer::MissHit(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}

Renderer::QuadraticResult Renderer::SolveQuadratic(const Ray& ray, const Sphere& sphere)
{
	Renderer::QuadraticResult result;

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
		float t0 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		float t1 = (-b + glm::sqrt(discriminant)) / (2.0f * a);

		if (t0 < 0 && t1 > 0)
		{
			result.T[0] = t1;
			result.T[1] = t0;
			result.IsFrontFace = false;
		}
		else
		{
			result.T[0] = t0;
			result.T[1] = t1;
			result.IsFrontFace = true;
		}
	}

	return result;
}