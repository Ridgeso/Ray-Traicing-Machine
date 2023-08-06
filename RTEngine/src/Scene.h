#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace RT::Render
{

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
