#pragma once
#include "system.hpp"

#include "../components/terrain.hpp"
#include "../components/interTerrain.h"

#include "../systems/graphSystem.hpp"

#include "../lve_window.hpp"

#include "../ecs/coordinator.hpp"

#include <glm\glm.hpp>

class InterractionTerrain : public System
{
	public :
		void init(GLFWwindow* glfw_window);
		void update(float dt);

	private :
		GLFWwindow* window;
		void raycast(glm::vec3 start, glm::vec3 direction, InterTerrain& it);
};