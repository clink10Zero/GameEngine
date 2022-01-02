#include "./physicsSystem.hpp"

#include "../ecs/coordinator.hpp"
#include "../components/Transform.hpp"
#include "../components/RigidBody.hpp"
#include "../components/mesh.hpp"
#include "../components/MotionControl.hpp"

#include <iostream>

extern Coordinator gCoordinator;

void PhysiqueSystem::Init(GLFWwindow* glfw_window) {
	 window = glfw_window;
	 for (auto const& go : mGameObject) {
		 calculeAABB(go);
	 }
}

void PhysiqueSystem::Update(float dt){

	for (auto const& go : mGameObject){
		moveInPlanXZ(dt, go);
		testCollision(dt, go);
	}
}

void PhysiqueSystem::makeMove(float dt, GameObject  go){
	Transform& transform = gCoordinator.GetCompenent<Transform>(go);
	if (gCoordinator.HaveComponent<RigidBody>(go)){
		RigidBody& rb = gCoordinator.GetCompenent<RigidBody>(go);
		
		transform.translation += rb.velocity * dt;
		rb.velocity += rb.forceGravity * dt;
	}
	if (gCoordinator.HaveComponent<MotionControl>(go)) {
		MotionControl& mc = gCoordinator.GetCompenent<MotionControl>(go);
		transform.translation += mc.movement;
	}

}

void PhysiqueSystem::moveInPlanXZ(float dt, GameObject go) {
	if (gCoordinator.HaveComponent<MotionControl>(go)){
		Transform& transform = gCoordinator.GetCompenent<Transform>(go);
		MotionControl& mc = gCoordinator.GetCompenent<MotionControl>(go);

		mc.movement = { 0.f, 0.f, 0.f };
		glm::vec3 rotate{ .0f };
		if (glfwGetKey(window, mc.keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(window, mc.keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(window, mc.keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(window, mc.keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			transform.rotation += mc.lookSpeed * dt * glm::normalize(rotate);
		}

		transform.rotation.x = glm::clamp(transform.rotation.x, -1.5f, 1.5f);
		transform.rotation.y = glm::mod(transform.rotation.y, glm::two_pi<float>());

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
		//if (glfwGetKey(window, mc.keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			mc.movement = mc.moveSpeed * dt * glm::normalize(moveDir);
		}
	}
}

void PhysiqueSystem::testCollision(float dt, GameObject go){

	if (gCoordinator.HaveComponent<RigidBody>(go)){
		AABB aabbDeplacement = calculeAABBDeplacement(go, dt);

		bool move = true;
		for (auto const& go2 : mGameObject){
			if (go2 != go){
				AABB aabbDepalcement2 = calculeAABBDeplacement(go2, dt);

				if ((aabbDeplacement.min.x <= aabbDepalcement2.min.x && aabbDeplacement.max.x >= aabbDepalcement2.min.x ||
					aabbDeplacement.max.x >= aabbDepalcement2.max.x && aabbDeplacement.min.x <= aabbDepalcement2.max.x ||
					aabbDeplacement.min.x >= aabbDepalcement2.min.x && aabbDeplacement.max.x <= aabbDepalcement2.max.x) &&
					(aabbDeplacement.min.y <= aabbDepalcement2.min.y && aabbDeplacement.max.y >= aabbDepalcement2.min.y ||
						aabbDeplacement.max.y >= aabbDepalcement2.max.y && aabbDeplacement.min.y <= aabbDepalcement2.max.y ||
						aabbDeplacement.min.y >= aabbDepalcement2.min.y && aabbDeplacement.max.y <= aabbDepalcement2.max.y) &&
					(aabbDeplacement.min.z <= aabbDepalcement2.min.z && aabbDeplacement.max.z >= aabbDepalcement2.min.z ||
						aabbDeplacement.max.z >= aabbDepalcement2.max.z && aabbDeplacement.min.z <= aabbDepalcement2.max.z ||
						aabbDeplacement.min.z >= aabbDepalcement2.min.z && aabbDeplacement.max.z <= aabbDepalcement2.max.z)
					) {
					//TODO gérer la collision

					AABB& aabb = gCoordinator.GetCompenent<AABB>(go);
					AABB& aabb2 = gCoordinator.GetCompenent<AABB>(go2);

					Transform& t = gCoordinator.GetCompenent<Transform>(go);
					t.translation.y += -aabb.max.y + aabb2.min.y;
					RigidBody& rb = gCoordinator.GetCompenent<RigidBody>(go);
					rb.velocity = { 0,0,0 };
				}
			}
		}

		makeMove(dt, go);

	}
}
void PhysiqueSystem::calculeAABB(GameObject go){

	auto& mesh = gCoordinator.GetCompenent<Mesh>(go);
	auto& aabb = gCoordinator.GetCompenent<AABB>(go);
	auto& transform = gCoordinator.GetCompenent<Transform>(go);
	//TODO getWorldTransform

	glm::vec3 currentMax = transform.mat4() * glm::vec4(mesh.data->getPostionVertex(0), 1.f);
	glm::vec3 currentMin = transform.mat4() * glm::vec4(mesh.data->getPostionVertex(0), 1.f);

	for (int i = 1; i < mesh.data->getVertexSize(); i++){
		glm::vec3 tmp = transform.mat4() * glm::vec4(mesh.data->getPostionVertex(i), 1.f);

		if (currentMax.x < tmp.x)
			currentMax.x = tmp.x;
		if (currentMax.y < tmp.y)
			currentMax.y = tmp.y;
		if (currentMax.z < tmp.z)
			currentMax.z = tmp.z;

		if (currentMin.x > tmp.x)
			currentMin.x = tmp.x;
		if (currentMin.y > tmp.y)
			currentMin.y = tmp.y;
		if (currentMin.z > tmp.z)
			currentMin.z = tmp.z;
	}

	aabb.max = currentMax;
	aabb.min = currentMin;
}

AABB PhysiqueSystem::calculeAABBDeplacement(GameObject go, float dt){
	calculeAABB(go);
	AABB& aabb = gCoordinator.GetCompenent<AABB>(go);

	if (gCoordinator.HaveComponent<RigidBody>(go)){
		RigidBody& rb = gCoordinator.GetCompenent<RigidBody>(go);
		Transform move{};
		move.translation = rb.velocity * dt;

		AABB aabbAfterMove{};
		aabbAfterMove.max = aabb.max + move.translation;
		aabbAfterMove.min = aabb.min + move.translation;

		glm::vec3 max = aabb.max;
		glm::vec3 min = aabb.min;

		if (max.x < aabbAfterMove.max.x)
			max.x = aabbAfterMove.max.x;
		if (max.y < aabbAfterMove.max.y)
			max.y = aabbAfterMove.max.y;
		if (max.z < aabbAfterMove.max.z)
			max.z = aabbAfterMove.max.z;

		if (min.x > aabbAfterMove.min.x)
			min.x = aabbAfterMove.min.x;
		if (min.y > aabbAfterMove.min.y)
			min.y = aabbAfterMove.min.y;
		if (min.z > aabbAfterMove.min.z)
			min.z = aabbAfterMove.min.z;

		return AABB{ max , min };
	}

	return aabb;
}


