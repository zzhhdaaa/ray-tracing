#include "Renderer.h"

#include "Walnut/Random.h"

#define PI 3.14159265

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

void Renderer::Render()
{
	float aspectRatio = (float)m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();
	//render every pixel
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; //remap to [-1, 1]
			coord.x *= aspectRatio;
			m_ImageData[x + y * m_FinalImage->GetWidth()] = RenderPerPixel(coord);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::RenderPerPixel(glm::vec2 coord)
{
	// ray
	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	rayDirection = glm::normalize(rayDirection); //don't really have to normalize

	// sphere
	glm::vec3 sphereOrigin(0.0f, 0.0f, 0.0f);
	float radius = 0.5f;

	// plane
	glm::vec3 planeOrigin(0.0f, -0.5f, 0.0f);
	//glm::vec3 planeNormal(0.0f, 1.0f, 0.0f);

	// light
	glm::vec3 lightDir(1.0f, -1.0f, -1.0f);
	lightDir = glm::normalize(lightDir);

	// ray = p + td
	// p = ray origin, d = ray direction, o = circle origin, r = radius, t = hit distance
	// (dx^2 + dy^2)t^2 + 2(pxdx + pydy - (dxox + dyoy))t + (px^2 + py^2 + ox^2 + oy^2 - 2(pxox + pyoy) - r^2) = 0
	// delta = b^2 - 4ac

	// calculating if the camera ray intersects with the sphere
	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * (glm::dot(rayOrigin, rayDirection) - glm::dot(rayDirection, sphereOrigin));
	float c = glm::dot(rayOrigin, rayOrigin) + glm::dot(sphereOrigin, sphereOrigin) - 2.0f * glm::dot(rayOrigin, sphereOrigin) - radius * radius;

	float delta = b * b - 4.0f * a * c;

	// if intersects, draw the pixel as sphere
	if (delta >= 0.0f)
	{
		float t[] = {
			(-b - pow(delta, 0.5f)) / (2.0f * a),
			(-b + pow(delta, 0.5f)) / (2.0f * a)
		};

		if (t[0] - t[1] >= -0.2f)
		{
			return 0xffffffff;
		}

		for (int i = 0; i < 2; i++)
		{
			glm::vec3 hitPosition = rayOrigin + rayDirection * t[i];
			glm::vec3 hitNormal = glm::normalize(hitPosition - sphereOrigin);

			//For two vectors a, b
			//the dot product, dot(a, b) = |a||b|cos¦È
			//which represents the similarity between two vectors' directions.
			//In this case, the more the hitNormal faces where the light comes from, 
			//the multiplier gets larger.
			float lightMultiplier = std::max(glm::dot(hitNormal, -lightDir), 0.0f);

			uint8_t r = (uint8_t)((hitNormal.x / 2.0f + 0.5f) * lightMultiplier * 255.0f);
			uint8_t g = (uint8_t)((hitNormal.y / 2.0f + 0.5f) * lightMultiplier * 255.0f);
			uint8_t b = (uint8_t)((hitNormal.z / 2.0f + 0.5f) * lightMultiplier * 255.0f);
			return 0xff000000 | b << 16 | g << 8 | r;
			//uint8_t r = (uint8_t)(97.0f * lightMultiplier);
			//uint8_t g = (uint8_t)(78.0f * lightMultiplier);
			//uint8_t b = (uint8_t)(113.0f * lightMultiplier);
			//return 0xff000000 | b << 16 | g << 8 | r;
		}
	}

	// calculating if the camera ray intersects with the plane
	float t = (planeOrigin.y - rayOrigin.y) / (float)rayDirection.y;

	if (t > 0)
	{
		// calculate if the camera ray intersects with the shadow area
		// set the intersect point of camera and plane as an origin, shot a ray in -lightDir, see if it intersects with the sphere
		glm::vec3 intersectOrigin = rayOrigin + rayDirection * t;
		float a1 = glm::dot(-lightDir, -lightDir);
		float b1 = 2.0f * (glm::dot(intersectOrigin, -lightDir) - glm::dot(-lightDir, sphereOrigin));
		float c1 = glm::dot(intersectOrigin, intersectOrigin) + glm::dot(sphereOrigin, sphereOrigin) - 2.0f * glm::dot(intersectOrigin, sphereOrigin) - radius * radius;

		float delta1 = b1 * b1 - 4.0f * a1 * c1;

		if (delta1 > 0.0f)
		{
			float alphaRaw = (1.0f / (delta1 + 1.0f));
			uint8_t alpha = (uint8_t)(alphaRaw * 0.3f * 255.0f);
			return 0xff000000 | alpha << 16 | alpha << 8 | alpha;
		}

		return 0xffe592a3;
	}

	uint8_t r = (uint8_t)((coord.y / 2.0f + 0.5f) * 255.0f);
	uint8_t g = (uint8_t)((coord.y / 2.0f + 0.5f) * 255.0f);

	return 0xff000000 | (g << 8) | r;
}