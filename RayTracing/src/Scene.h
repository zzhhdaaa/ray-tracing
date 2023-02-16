#pragma once

#include <vector>

#include "Walnut/Random.h"

#include "Geometry.h"
#include "Material.h"

class Scene
{
public:
	Scene();

	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};