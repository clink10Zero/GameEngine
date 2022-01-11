#include "./viewSystem.hpp"
#include "./graphSystem.hpp"

extern std::shared_ptr<GraphSystem> graphSystem;
void ViewSystem::Init(float aspectRatio, GLFWwindow* glfw_window){
	window = glfw_window;
	currentCam = *mGameObject.begin();
	setUpCamera(aspectRatio);
}

void ViewSystem::Update(float aspectRatio) {
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		nextCam();//touche "A" c'est basé sur du qwerty apparmemment...
	}
	setUpCamera(aspectRatio);
}

void ViewSystem::setUpCamera(float aspectRatio) {
	Camera& camera = gCoordinator.GetCompenent<Camera>(currentCam);
	Transform worldTransform = graphSystem->getWorldPosition(currentCam);

	camera.setViewYXZ(worldTransform.translation, worldTransform.rotation);

	camera.setPerspectiveProjection(glm::radians(50.f), aspectRatio, .1f, 100.f);
}
void ViewSystem::nextCam() {
	if (next(mGameObject.find(currentCam), 1) != mGameObject.end()){
		currentCam = *next(mGameObject.find(currentCam), 1);
	}
	else {
		currentCam = *mGameObject.begin();
	}
}


