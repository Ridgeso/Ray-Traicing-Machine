#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace RT::Render
{

	#pragma pack(1)
	struct Material
	{
		glm::vec3 albedo;
		float padding1[1];
		glm::vec3 emissionColor;
		float roughness;
		float metalic;
		float emissionPower;
		float refractionRatio;
		float padding2[1];
	};

	#pragma pack(1)
	struct Sphere
	{
		glm::vec3 position;
		float radius;
		int32_t materialId;
		float padding[3];
	};

	struct Scene
	{
		std::vector<Material> materials;
		std::vector<Sphere> spheres;
	};

}
