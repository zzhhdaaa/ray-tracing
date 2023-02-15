#include "Scene.h"

Scene::Scene()
{
	Material& redSphere = Materials.emplace_back();
	redSphere.Albedo = glm::vec3(1.0f, 0.4f, 0.4f);
	redSphere.Roughness = 0.5f;
	redSphere.Opacity = 0.3f;

	Material& greenSphere = Materials.emplace_back();
	greenSphere.Albedo = glm::vec3(0.4f, 1.0f, 0.4f);
	greenSphere.Roughness = 0.0f;
	greenSphere.Opacity = 0.0f;

	Material& blueSphere = Materials.emplace_back();
	blueSphere.Albedo = glm::vec3(0.4f, 0.4f, 1.0f);
	blueSphere.Roughness = 0.02f;
	blueSphere.Opacity = 1.0f;

	Material& graySphere = Materials.emplace_back();
	graySphere.Albedo = glm::vec3(0.9f, 0.9f, 0.9f);
	graySphere.Roughness = 0.15f;

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
}
