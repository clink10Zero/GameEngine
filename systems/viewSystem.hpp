#pragma once

#include "system.hpp"
#include "../lve_renderer.hpp"
#include "../ecs/coordinator.hpp"
#include "../components/Camera.hpp"

extern Coordinator gCoordinator;

class ViewSystem : public System
{
	public:
		void Init(float aspectRatio);
		void Update(float aspectRatio);

		const glm::mat4& getProjection() const {
			return gCoordinator.GetCompenent<Camera>(currentCam).projectionMatrix;
		}

		const glm::mat4& getView()const {
			return gCoordinator.GetCompenent<Camera>(currentCam).viewMatrix;
		}
		
		void setUpCamera(float aspectRatio);
	private :
		GameObject currentCam;
};
