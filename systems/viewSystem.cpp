#include "./viewSystem.hpp"

#include "./graphSystem.hpp"

#include "../components/ViewControl.hpp"
#include "../components/MotionControl.hpp"

extern std::shared_ptr<GraphSystem> graphSystem;
void ViewSystem::Init(float aspectRatio, GLFWwindow* glfw_window) {
	window = glfw_window;
	currentCam = *mGameObject.begin();
	setUpCamera(aspectRatio);
}

void ViewSystem::Update(float aspectRatio, float dt) {
	
	for (const auto& go : mGameObject)
	{
		moveInPlanXZ(dt, go);
	}

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		nextCam();
	}
	setUpCamera(aspectRatio);
	lookAround(dt);
}

void ViewSystem::setUpCamera(float aspectRatio) {
	Camera& camera = gCoordinator.GetCompenent<Camera>(currentCam);
	Transform worldTransform = graphSystem->getWorldPosition(currentCam);

	camera.setViewYXZ(worldTransform.translation, worldTransform.rotation);

	camera.setPerspectiveProjection(glm::radians(50.f), aspectRatio, .1f, 100.f);
}

void ViewSystem::nextCam() {
	if (next(mGameObject.find(currentCam), 1) != mGameObject.end()) {
		currentCam = *next(mGameObject.find(currentCam), 1);
	}
	else {
		currentCam = *mGameObject.begin();
	}
}

void ViewSystem::lookAround(float dt) {
	Transform& transform = gCoordinator.GetCompenent<Transform>(currentCam);
	if (gCoordinator.HaveComponent<ViewControl>(currentCam)) {
		ViewControl& vc = gCoordinator.GetCompenent<ViewControl>(currentCam);
		glm::vec3 rotate{ .0f };
		if (glfwGetKey(window, vc.keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(window, vc.keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(window, vc.keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(window, vc.keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			transform.rotation += vc.lookSpeed * dt * glm::normalize(rotate);
		}

		transform.rotation.x = glm::clamp(transform.rotation.x, -1.5f, 1.5f);
		transform.rotation.y = glm::mod(transform.rotation.y, glm::two_pi<float>());
	}

}

void ViewSystem::moveInPlanXZ(float dt, GameObject go) {
	if (gCoordinator.HaveComponent<MotionControl>(go)) {
		Transform& transform = gCoordinator.GetCompenent<Transform>(go);
		MotionControl& mc = gCoordinator.GetCompenent<MotionControl>(go);

		mc.movement = { 0.f, 0.f, 0.f };

		float yaw = transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), .0f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, .0f, -forwardDir.x };
		const glm::vec3 upDir{ .0f, -1.f, .0f };

		glm::vec3 moveDir{ .0f };
		if (glfwGetKey(window, mc.keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, mc.keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window, mc.keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, mc.keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, mc.keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, mc.keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			mc.movement = mc.moveSpeed * dt * glm::normalize(moveDir);
		}

		transform.translation += mc.movement;
	}
}