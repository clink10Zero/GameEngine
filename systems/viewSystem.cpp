#include "./viewSystem.hpp"
#include "./graphSystem.hpp"

extern std::shared_ptr<GraphSystem> graphSystem;
void ViewSystem::Init(float aspectRatio){
	for (auto const& go : mGameObject) {
		currentCam = go;
		break;
	}
	setUpCamera(aspectRatio);
}

void ViewSystem::Update(float aspectRatio) {
	setUpCamera(aspectRatio);
}

void ViewSystem::setUpCamera(float aspectRatio) {
	Camera& camera = gCoordinator.GetCompenent<Camera>(currentCam);
	Transform worldTransform = graphSystem->getWorldPosition(currentCam);

	camera.setViewYXZ(worldTransform.translation, worldTransform.rotation);

	camera.setPerspectiveProjection(glm::radians(50.f), aspectRatio, .1f, 100.f);
}

