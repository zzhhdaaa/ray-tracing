#pragma once

#include "Walnut/Application.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class RenderLayer : public Walnut::Layer
{
public:
	RenderLayer()
		: m_Camera(45.0f, 0.1f, 100.0f)
	{
		{
			Sphere sphere;
			sphere.Origin = { 0.0f, 0.0f, 0.0f };
			sphere.Radius = 0.5f;
			sphere.Albedo = glm::vec3(1.0f, 0.4f, 0.4f);
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Origin = { 2.0f, 0.0f, -5.0f };
			sphere.Radius = 1.0f;
			sphere.Albedo = glm::vec3(0.4f, 1.0f, 0.4f);
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Origin = { -2.0f, -1.0f, -4.0f };
			sphere.Radius = 1.5f;
			sphere.Albedo = glm::vec3(0.4f, 0.4f, 1.0f);
			m_Scene.Spheres.push_back(sphere);
		}
	}
	virtual void OnUpdate(float ts) override;
	virtual void OnUIRender() override;

	void Render();
private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	float m_LastRenderTime = 0.0f;
};