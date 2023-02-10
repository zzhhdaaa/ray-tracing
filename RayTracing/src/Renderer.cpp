#include "Renderer.h"

#include "Walnut/Random.h"

#define PI 3.14159265

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
	Ray ray;
	ray.Origin = camera.GetPosition();

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			ray.Direction = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];

			glm::vec4 color = TraceRay(scene, ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			// pass the color to image data
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	if (scene.Spheres.size() == 0)
	{
		float rgbScale = (ray.Direction.y * 0.5f + 0.5f) * 1.3f;
		return glm::vec4(0.95f * rgbScale, 0.89f * rgbScale, 0.74f * rgbScale, 1);
	}

	// plane
	glm::vec3 planeOrigin(0.0f, -0.5f, 0.0f);
	//glm::vec3 planeNormal(0.0f, 1.0f, 0.0f);

	// light
	glm::vec3 lightDir(1.0f, -1.0f, -1.0f);
	lightDir = glm::normalize(lightDir);

	// closet hit point
	const Sphere* closestSphere = nullptr;
	float closestT = FLT_MAX;
	float closestDiscriminant = FLT_MAX;

	for (const Sphere& sphere : scene.Spheres)
	{
		// sphere
		glm::vec3 sphereOrigin = sphere.Origin;
		float sphereRadius = sphere.Radius;

		// ray = p + td
		// p = ray origin, d = ray direction, o = circle origin, r = radius, t = hit distance
		// (dx^2 + dy^2)t^2 + 2(pxdx + pydy - (dxox + dyoy))t + (px^2 + py^2 + ox^2 + oy^2 - 2(pxox + pyoy) - r^2) = 0
		// discriminant = b^2 - 4ac
		// t = (-b +- sqrt(discriminant)) / 2a

		// calculating if the camera ray intersects with the sphere
		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * (glm::dot(ray.Origin, ray.Direction) - glm::dot(ray.Direction, sphereOrigin));
		float c = glm::dot(ray.Origin, ray.Origin) + glm::dot(sphereOrigin, sphereOrigin) - 2.0f * glm::dot(ray.Origin, sphereOrigin) - sphereRadius * sphereRadius;

		float discriminant = b * b - 4.0f * a * c;

		// if intersects, draw the pixel as sphere
		if (discriminant >= 0.0f)
		{
			// calculate the two hit points
			float t[] = {
				(-b - glm::sqrt(discriminant)) / (2.0f * a),
				(-b + glm::sqrt(discriminant)) / (2.0f * a)
			};

			if (t[0] >= 0 && t[0] < closestT)
			{
				closestT = t[0];
				closestSphere = &sphere;
				closestDiscriminant = discriminant;
			}
		}
	}

	// calculating if the camera ray intersects with the plane
	float t = (planeOrigin.y - ray.Origin.y) / (float)ray.Direction.y;

	// hit plane AND hit plane closer than closestT(could be hitting sphere or FLT_MAX)
	// draw plane or shadow
	if (t > 0 && t < closestT)
	{
		for (const Sphere& sphere : scene.Spheres)
		{
			// sphere
			glm::vec3 sphereOrigin = sphere.Origin;
			float sphereRadius = sphere.Radius;

			if (t > 0)
			{
				// calculate if the camera ray intersects with the shadow area
				// set the intersect point of camera and plane as a new origin, shot a ray in -lightDir direction, see if it intersects with the sphere
				glm::vec3 intersectOrigin = ray.Origin + ray.Direction * t;

				float a = glm::dot(-lightDir, -lightDir);
				float b = 2.0f * (glm::dot(intersectOrigin, -lightDir) - glm::dot(-lightDir, sphereOrigin));
				float c = glm::dot(intersectOrigin, intersectOrigin) + glm::dot(sphereOrigin, sphereOrigin) - 2.0f * glm::dot(intersectOrigin, sphereOrigin) - sphereRadius * sphereRadius;

				float discriminant = b * b - 4.0f * a * c;

				if (discriminant >= 0.0f )
				{
					float t[] = {
						(-b - glm::sqrt(discriminant)) / (2.0f * a),
						(-b + glm::sqrt(discriminant)) / (2.0f * a)
					};

					if (t[0] >= 0.0f)
					{
						// DRAW shadow
						float grayScale = (1.0f / (discriminant + 1.0f)) * 0.3f;
						return glm::vec4(grayScale, grayScale, grayScale, 1);
					}
				}

			}
		}
		// DRAW plane
		return glm::vec4(0.51f, 0.38f, 0.64f, 1);
	}
	else if (closestSphere == nullptr)
	{
		// DRAW background
		float rgbScale = (ray.Direction.y * 0.5f + 0.5f) * 1.3f;
		return glm::vec4(0.95f * rgbScale, 0.89f * rgbScale, 0.74f * rgbScale, 1);
	}
	else
	{
		// DRAW outline
		if (closestDiscriminant <= closestSphere->Radius * 0.1f)
		{
			return glm::vec4(1.0f);
		}

		// DRAW sphere
		// calculate the closest hit point position and normal
		glm::vec3 hitPosition = ray.Origin + ray.Direction * closestT;
		glm::vec3 hitNormal = glm::normalize(hitPosition - closestSphere->Origin);

		// For two vectors a, b
		// the dot product, dot(a, b) = |a||b|cos¦È
		// which represents the similarity between two vectors' directions.
		// In this case, the more the hitNormal faces where the light comes from, 
		// the multiplier gets larger.
		float lightMultiplier = std::max(glm::dot(hitNormal, -lightDir), 0.0f);

		// Visualize the normal
		//glm::vec3 sphereColor = (hitNormal * 0.5f + 0.5f) * lightMultiplier;
		glm::vec3 sphereColor = (hitNormal * 0.5f + 0.5f) * closestSphere->Albedo * lightMultiplier;

		return glm::vec4(sphereColor, 1);
	}
}