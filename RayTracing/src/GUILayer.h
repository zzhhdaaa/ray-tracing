#pragma once

#include "Walnut/Application.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class GUILayer : public Walnut::Layer
{
public:
	GUILayer();

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