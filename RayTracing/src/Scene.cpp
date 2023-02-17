#include "Scene.h"

Scene::Scene()
{
	Material& redSphere = Materials.emplace_back();
	redSphere.Albedo = glm::vec3(1.0f, 0.4f, 0.4f);
	redSphere.Roughness = 0.0f;
	redSphere.Opacity = 1.0f;
	redSphere.Texture = std::make_shared<SolidColor>(1.0f, 0.4f, 0.4f);
	//redSphere.Texture.SetColor(1.0f, 0.4f, 0.4f);

	Material& greenSphere = Materials.emplace_back();
	greenSphere.Albedo = glm::vec3(0.4f, 1.0f, 0.4f);
	greenSphere.Roughness = 0.2f;
	greenSphere.Opacity = 0.0f;
	greenSphere.Texture = std::make_shared<SolidColor>(0.4f, 1.0f, 0.4f);
	//greenSphere.Texture.SetColor(0.4f, 1.0f, 0.4f);

	Material& blueSphere = Materials.emplace_back();
	blueSphere.Albedo = glm::vec3(0.4f, 0.4f, 1.0f);
	blueSphere.Roughness = 0.5f;
	blueSphere.Opacity = 1.0f;
	//blueSphere.Texture = std::make_shared<CheckerTexture>(glm::vec3(0.4f, 0.4f, 1.0f), glm::vec3(1.0f, 0.4f, 0.4f));
	blueSphere.Texture = std::make_shared<ImageTexture>("img/earthmap.jpg");
	//blueSphere.Texture = std::make_shared<SolidColor>(0.4f, 0.4f, 1.0f);
	//blueSphere.Texture.SetColor(0.4f, 0.4f, 1.0f);

	Material& graySphere = Materials.emplace_back();
	graySphere.Albedo = glm::vec3(0.9f, 0.9f, 0.9f);
	graySphere.Roughness = 0.15f;
	graySphere.Texture = std::make_shared<CheckerTexture>(glm::vec3(0.4f, 0.4f, 1.0f), glm::vec3(1.0f, 0.4f, 0.4f));
	//graySphere.Texture = std::make_shared<SolidColor>(0.9f, 0.9f, 0.9f);
	//graySphere.Texture.SetColor(0.9f, 0.9f, 0.9f);

	{
		Sphere sphere;
		sphere.Origin = { 0.0f, -0.4f, 0.0f };
		sphere.Radius = 0.6f;
		sphere.MaterialIndex = 0;
		Spheres.push_back(sphere);
	}
	{
		Sphere sphere;
		sphere.Origin = { 2.0f, 0.5f, -3.0f };
		sphere.Radius = 1.0f;
		sphere.MaterialIndex = 1;
		Spheres.push_back(sphere);
	}
	{
		Sphere sphere;
		sphere.Origin = { -2.0f, 1.0f, -3.0f };
		sphere.Radius = 1.5f;
		sphere.MaterialIndex = 2;
		Spheres.push_back(sphere);
	}
	{
		Sphere sphere;
		sphere.Origin = { 0.0f, -1001.0f, 0.0f };
		sphere.Radius = 1000.0f;
		sphere.MaterialIndex = 3;
		Spheres.push_back(sphere);
	}

	//for (int i = 0; i < 12; i++)
	//{
	//	Material material;
	//	material.Albedo = Walnut::Random::Vec3();
	//	material.Roughness = Walnut::Random::Float();
	//	material.Opacity = Walnut::Random::Float();
	//	Materials.push_back(material);
	//}

	//for (int i = 0; i < 24; i++)
	//{
	//	Sphere sphere;
	//	float x = Walnut::Random::Float(-4.0f, 4.0f);
	//	float y = -0.8f;
	//	float z = Walnut::Random::Float(-6.0f, 2.0f);
	//	sphere.Origin = { x, y, z };
	//	sphere.Radius = Walnut::Random::Float(0.1f, 0.2f);
	//	sphere.MaterialIndex = (int)Walnut::Random::UInt(0, Materials.size());
	//	Spheres.push_back(sphere);
	//}
}
