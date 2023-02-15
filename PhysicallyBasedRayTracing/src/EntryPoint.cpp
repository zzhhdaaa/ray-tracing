#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "GUILayer.h"

#include "core/pbrt.h"
#include "core/transform.h"

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Vector3<Float> v(1, 5, 9.8f);
	std::cout << v.x << std::endl;
	std::cout << v.y << std::endl;
	std::cout << v.z << std::endl;

	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";
	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<GUILayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});

	return app;
}