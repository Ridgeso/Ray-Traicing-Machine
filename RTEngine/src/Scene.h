#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace RT::Render
{

	struct Ray
	{
		glm::vec3 Origin;
		glm::vec3 Direction;
	};

	struct Material
	{
		glm::vec3 Albedo;
		glm::vec3 EmmisionColor;
		float Roughness;
		float Metalic;
		float EmmisionPower;
	};

	glm::vec3 GetEmmision(const Material& mat);

	struct Sphere
	{
		glm::vec3 Position;
		float Radius;
		int32_t MaterialId;
	};

	struct Scene
	{
		std::vector<Material> Materials;
		std::vector<Sphere> Spheres;
	};

}
