#pragma once

#include "system.hpp"
#include "../lve_window.hpp"
#include "../components/aabb.hpp"

class PhysiqueSystem : public System
{
	public:
		void Init(GLFWwindow* glfw_window);

		void Update(float dt);

		void makeMove(float dt, GameObject go);
		void moveInPlanXZ(float dt, GameObject go);
		void testCollision(float dt, GameObject go);
		void calculeAABB(GameObject go);
		AABB calculeAABBDeplacement(GameObject go, float dt);

	private:
		GLFWwindow* window;	
};
