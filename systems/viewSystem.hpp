#pragma once

#include "system.hpp"
#include "../lve_renderer.hpp"
#include "../ecs/coordinator.hpp"
#include "../components/Camera.hpp"

extern Coordinator gCoordinator;

class ViewSystem : public System
{
	public:
		void Init(float aspectRatio, GLFWwindow* glfw_window);
		void Update(float aspectRatio, float dt);

		const glm::mat4& getProjection() const {
			return gCoordinator.GetCompenent<Camera>(currentCam).projectionMatrix;
		}

		const glm::mat4& getView()const {
			return gCoordinator.GetCompenent<Camera>(currentCam).viewMatrix;
		}
		
		void setUpCamera(float aspectRatio);
		void lookAround(float dt);
		void nextCam();
		
	private :
		GameObject currentCam;
		GLFWwindow* window;
};
