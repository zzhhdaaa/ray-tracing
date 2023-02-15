#pragma once

#include <vector>

#include "Geometry.h"
#include "Material.h"

class Scene
{
public:
	Scene();

	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};