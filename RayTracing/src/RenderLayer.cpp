#include "RenderLayer.h"

void RenderLayer::OnUpdate(float ts)
{
	m_Camera.OnUpdate(ts);
}

void RenderLayer::OnUIRender()
{
	// showing render time
	ImGui::Begin("Settings");
	ImGui::Text("Last render: %.3fms", m_LastRenderTime);
	if (ImGui::Button("Render"))
	{
		Render();
	}
	ImGui::End();

	// showing sphere controls
	ImGui::Begin("Spheres");
	for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
	{
		ImGui::PushID(i);

		Sphere& sphere = m_Scene.Spheres[i];
		ImGui::DragFloat3("Sphere Origin", glm::value_ptr(sphere.Origin), 0.1f);
		ImGui::DragFloat("Sphere Radius", &sphere.Radius, 0.1f);
		ImGui::ColorEdit3("Sphere Albedo", glm::value_ptr(sphere.Albedo));

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

void RenderLayer::Render()
{
	Timer timer;

	m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
	m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
	m_Renderer.Render(m_Scene, m_Camera);

	m_LastRenderTime = timer.ElapsedMillis();
}