#include "RendererUI.h"

void RendererUI::OnUpdate(float ts)
{
	if(m_Camera.OnUpdate(ts))
		m_Renderer.ResetAccumulationFrame();
}

void RendererUI::OnUIRender()
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
	for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
	{
		ImGui::PushID(i);

		Sphere& sphere = m_Scene.Spheres[i];
		if (ImGui::DragFloat3("Sphere Origin", glm::value_ptr(sphere.Origin), 0.1f))
			m_Renderer.ResetAccumulationFrame();
		if (ImGui::DragFloat("Sphere Radius", &sphere.Radius, 0.1f))
			m_Renderer.ResetAccumulationFrame();
		if (ImGui::DragInt("Sphere Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1))
			m_Renderer.ResetAccumulationFrame();

		ImGui::Separator();

		ImGui::PopID();
	}
	ImGui::End();

	// showing material controls
	ImGui::Begin("Materials");
	for (size_t i = 0; i < m_Scene.Materials.size(); i++)
	{
		ImGui::PushID(i);

		Material& material = m_Scene.Materials[i];
		if (ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo)))
			m_Renderer.ResetAccumulationFrame();
		if (ImGui::DragFloat("Roughness", &material.Roughness, 0.01f, 0.0f, 1.0f))
			m_Renderer.ResetAccumulationFrame();
		if (ImGui::DragFloat("Metallic", &material.Metallic, 0.01f, 0.0f, 1.0f))
			m_Renderer.ResetAccumulationFrame();
		if (ImGui::DragFloat("Opacity", &material.Opacity, 0.01f, 0.0f, 1.0f))
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

void RendererUI::Render()
{
	Timer timer;

	m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
	m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
	m_Renderer.Render(m_Scene, m_Camera);

	m_LastRenderTime = timer.ElapsedMillis();
}