#pragma once

#include <glm/glm.hpp>


namespace RayTracing {

	struct Sphere
	{
		glm::vec3 Position;
		float Radius = 0.5f;

		glm::vec3 Albedo { 1.0f };
	};

}