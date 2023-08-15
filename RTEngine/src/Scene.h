#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace RT::Render
{

	#pragma pack(1)
	struct Material
	{
		glm::vec3 Albedo;
		float Roughness;
		float Metalic;
		float padding[3];
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
