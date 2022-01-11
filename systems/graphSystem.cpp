#include "./graphSystem.hpp"
#include "../ecs/coordinator.hpp"

extern Coordinator gCoordinator;

Transform GraphSystem::getWorldPosition(GameObject go){
	Transform worldPosition = gCoordinator.GetCompenent<Transform>(go);
	GameObject parent = gCoordinator.GetCompenent<Graph>(go).parent;

	while(parent != 0){
		worldPosition=worldPosition.combineWith(gCoordinator.GetCompenent<Transform>(parent));
		parent = gCoordinator.GetCompenent<Graph>(parent).parent;
	}

	return worldPosition;
}
