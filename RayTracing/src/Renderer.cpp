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
	//render every pixel
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; //remap to [-1, 1]
			m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)((coord.y/2.0f + 0.5f) * 255.0f);
	uint8_t g = (uint8_t)((coord.y/2.0f + 0.5f) * 255.0f);

	glm::vec3 rayOrigin(0.0f, 0.0f, 3.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	rayDirection = glm::normalize(rayDirection);

	glm::vec3 sphereOrigin(0.5f, 1.5f, 0.0f);
	float radius = 0.5f;
	// rayDirection = glm::normalize(rayDirection);

	// ray = p + td
	// p = ray origin, d = ray direction, o = circle origin, r = radius, t = hit distance
	// (dx^2 + dy^2)t^2 + 2(pxdx + pydy)t + (px^2 + py^2 - r^2) = 0
	// (dx^2 + dy^2)t^2 + 2(pxdx + pydy - (dxox + dyoy))t + (px^2 + py^2 + ox^2 + oy^2 - 2(pxox + pyoy) - r^2) = 0
	// delta = b^2 - 4ac

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * (glm::dot(rayOrigin, rayDirection) - glm::dot(rayDirection, sphereOrigin));
	float c = glm::dot(rayOrigin, rayOrigin) + glm::dot(sphereOrigin, sphereOrigin) - 2.0f * glm::dot(rayOrigin, sphereOrigin) - radius * radius;

	float delta = b * b - 4.0f * a * c;

	if (delta >= 0.0f)
	{
		float t1 = (-b + pow(delta, 0.5f)) / (2 * a);
		float t2 = (-b - pow(delta, 0.5f)) / (2 * a);

		uint8_t b = (uint8_t)(255.0f - (t2 / 2.0f) * 255.0f);

		return 0xff000000 | b << 16;
	}

	return 0xff000000 | (g << 8) | r;
}