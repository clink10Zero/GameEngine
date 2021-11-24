#pragma once

#include <glm/gtc/matrix_transform.hpp>

struct RigidBody
{
	glm::vec3 velocity;
	glm::vec3 acceleration;

	glm::vec3 forceGravity;
};