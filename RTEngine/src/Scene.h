#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace RT::Render
{

	#pragma pack(1)
	struct Material
	{
		glm::vec3 Albedo;
		float Padding1[1];
		glm::vec3 EmmisionColor;
		float Roughness;
		float Metalic;
		float EmmisionPower;
		float Padding2[2];
	};

	#pragma pack(1)
	struct Sphere
	{
		glm::vec3 Position;
		float Radius;
		int32_t MaterialId;
		float padding[3];
	};

	struct Scene
	{
		std::vector<Material> Materials;
		std::vector<Sphere> Spheres;
	};

}
