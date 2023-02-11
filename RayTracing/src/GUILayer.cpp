#include "GUILayer.h"

GUILayer::GUILayer()
	: m_Camera(45.0f, 0.1f, 100.0f)
{
	Material& redSphere = m_Scene.Materials.emplace_back();
	redSphere.Albedo = glm::vec3(1.0f, 0.4f, 0.4f);
	redSphere.Roughness = 0.2f;

	Material& greenSphere = m_Scene.Materials.emplace_back();
	greenSphere.Albedo = glm::vec3(0.4f, 1.0f, 0.4f);
	greenSphere.Roughness = 0.6f;

	Material& blueSphere = m_Scene.Materials.emplace_back();
	blueSphere.Albedo = glm::vec3(0.4f, 0.4f, 1.0f);
	blueSphere.Roughness = 0.3f;

	Material& graySphere = m_Scene.Materials.emplace_back();
	graySphere.Albedo = glm::vec3(0.1f, 0.2f, 0.3f);
	graySphere.Roughness = 0.8f;

	{
		Sphere sphere;
		sphere.Origin = { 0.0f, -0.5f, 0.0f };
		sphere.Radius = 0.5f;
		sphere.MaterialIndex = 0;
		m_Scene.Spheres.push_back(sphere);
	}
	{
		Sphere sphere;
		sphere.Origin = { 2.0f, 0.5f, -5.0f };
		sphere.Radius = 1.0f;
		sphere.MaterialIndex = 1;
		m_Scene.Spheres.push_back(sphere);
	}
	{
		Sphere sphere;
		sphere.Origin = { -2.0f, 0.0f, -4.0f };
		sphere.Radius = 1.5f;
		sphere.MaterialIndex = 2;
		m_Scene.Spheres.push_back(sphere);
	}
	{
		Sphere sphere;
		sphere.Origin = { 0.0f, -501.0f, 0.0f };
		sphere.Radius = 500.0f;
		sphere.MaterialIndex = 3;
		m_Scene.Spheres.push_back(sphere);
	}
}

void GUILayer::OnUpdate(float ts)
{
	if(m_Camera.OnUpdate(ts))
		m_Renderer.ResetAccumulationFrame();
}

void GUILayer::OnUIRender()
{
	// showing render time
	ImGui::Begin("Settings");
	ImGui::Text("Last render: %.3fms", m_LastRenderTime);
	if (ImGui::Button("Render"))
	{
		Render();
	}
	if (ImGui::Button("Reset"))
	{
		m_Renderer.ResetAccumulationFrame();
	}
	ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
	ImGui::End();

	// showing sphere controls
	ImGui::Begin("Spheres");
	//for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
	//{
	//	ImGui::PushID(i);

	//	Sphere& sphere = m_Scene.Spheres[i];
	//	ImGui::DragFloat3("Sphere Origin", glm::value_ptr(sphere.Origin), 0.1f);
	//	ImGui::DragFloat("Sphere Radius", &sphere.Radius, 0.1f);
	//	ImGui::DragInt("Sphere Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1);

	//	ImGui::Separator();

	//	ImGui::PopID();
	//}
	for (size_t i = 0; i < m_Scene.Materials.size(); i++)
	{
		ImGui::PushID(i);

		Material& material = m_Scene.Materials[i];
		if(ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo)))
			m_Renderer.ResetAccumulationFrame();
		if(ImGui::DragFloat("Roughness", &material.Roughness, 0.01f, 0.0f, 1.0f))
			m_Renderer.ResetAccumulationFrame();
		if(ImGui::DragFloat("Metallic", &material.Metallic, 0.01f, 0.0f, 1.0f))
			m_Renderer.ResetAccumulationFrame();

		ImGui::Separator();

		ImGui::PopID();
	}


	ImGui::End();

	// showing the render output
	ImGui::Begin("Viewport");
	m_ViewportWidth = ImGui::GetContentRegionAvail().x;
	m_ViewportHeight = ImGui::GetContentRegionAvail().y;

	auto image = m_Renderer.GetFinalImage();
	if (image)
		ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();

	Render();
}

void GUILayer::Render()
{
	Timer timer;

	m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
	m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
	m_Renderer.Render(m_Scene, m_Camera);

	m_LastRenderTime = timer.ElapsedMillis();
}