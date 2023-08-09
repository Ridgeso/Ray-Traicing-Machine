#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace RT::Render
{

	struct Ray
	{
		glm::vec3 origin;
		glm::vec3 direction;
	};

	struct Payload
	{
		glm::vec4 colour;
		float hitDistance;
		int32_t objectId;
	};

	struct Sphere
	{
		glm::vec3 position;
		float radius;
		glm::vec4 color;
	};

	struct Scene
	{
		std::vector<Sphere> Spheres;
	};

}
