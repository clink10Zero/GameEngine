#include "./physicsSystem.hpp"

#include "../ecs/coordinator.hpp"
#include "../components/Transform.hpp"
#include "../components/RigidBody.hpp"
#include "../components/mesh.hpp"
#include "../components/MotionControl.hpp"

#include <iostream>

#define INFINY std::numeric_limits<float>::infinity()

extern Coordinator gCoordinator;

enum eDirection { RIGHT, LEFT, TOP, BOTTOM, FRONT, BACK };

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

	AABB aabbDeplacement = calculeAABBDeplacement(go, dt);
	AABB& aabb = gCoordinator.GetCompenent<AABB>(go);

	glm::vec3 move{ 0.f,0.f,0.f };
	if (gCoordinator.HaveComponent<RigidBody>(go)) {
		RigidBody& rb = gCoordinator.GetCompenent<RigidBody>(go);
		rb.velocity += rb.forceGravity * dt;
		move += rb.velocity * dt;
	}

	if (gCoordinator.HaveComponent<MotionControl>(go)) {
		MotionControl& mc = gCoordinator.GetCompenent<MotionControl>(go);
		move += mc.movement;
	}

	if (move != glm::vec3(0.f, 0.f, 0.f)) {//pas de déplacement = pas de collision
		std::unordered_map<eDirection, glm::vec2> collisions{};
		collisions.insert({ RIGHT,{INFINY,INFINY} });
		collisions.insert({ LEFT,{INFINY,INFINY} });
		collisions.insert({ TOP,{INFINY,INFINY} });
		collisions.insert({ BOTTOM,{INFINY,INFINY} });
		collisions.insert({ FRONT,{INFINY,INFINY} });
		collisions.insert({ BACK,{INFINY,INFINY} });

		for (auto const& go2 : mGameObject) {
			if (go2 != go) {
				AABB aabbDepalcement2 = calculeAABBDeplacement(go2, dt);
				//TODO il est possible que ça soit simplifiable
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

					
					AABB& aabb2 = gCoordinator.GetCompenent<AABB>(go2);

					glm::vec3 dEntry;//distance pour entrée
					glm::vec3 dExit;//distance pour sortir

					dEntry.x = aabb2.min.x - aabb.max.x;
					dExit.x = aabb2.max.x - aabb.min.x;
					if(std::abs(aabb.max.x - aabb2.min.x)< std::abs(dEntry.x)){
						dEntry.x = aabb.max.x - aabb2.min.x;
						dExit.x = aabb.min.x - aabb2.max.x;
					}

					dEntry.y = aabb2.min.y - aabb.max.y;
					dExit.y = aabb2.max.y - aabb.min.y;
					if(std::abs(aabb.max.y - aabb2.min.y)< std::abs(dEntry.y)) {
						dEntry.y = aabb.max.y - aabb2.min.y;
						dExit.y = aabb.min.y - aabb2.max.y;
					}

					
					dEntry.z = aabb2.min.z - aabb.max.z;
					dExit.z = aabb2.max.z - aabb.min.z;
					if(std::abs(aabb.max.z - aabb2.min.z)< std::abs(dEntry.z)) {
						dEntry.z = std::abs(aabb.max.z - aabb2.min.z);
						dExit.z = std::abs(aabb.min.z - aabb2.max.z);
					}

					glm::vec3 tEntry;
					glm::vec3 tExit;
					if (move.x == 0.f) {
						tEntry.x = -INFINY;
						tExit.x = INFINY;
					}
					else {
						tEntry.x = dEntry.x / move.x;
						tExit.x = dExit.x / move.x;
					}
					if (move.y == 0.f) {
						tEntry.y = -INFINY;
						tExit.y = INFINY;
					}
					else {
						tEntry.y = dEntry.y / move.y;
						tExit.y = dExit.y / move.y;
					}
					if (move.z == 0.f) {
						tEntry.z = -INFINY;
						tExit.z = INFINY;
					}
					else {
						tEntry.z = dEntry.z / move.z;
						tExit.z = dExit.z / move.z;
					}

					/*In order to collide, both axes need to get collided .So, we will take the longest time to begin collision.
					And when end collision, we only need one of the axes exit collision, so we will take the quickest time to exit collision.*/
					float entryTime = max(max(tEntry.x, tEntry.y), tEntry.z);
					float exitTime = min(min(tExit.x, tExit.y), tExit.z);

					if (entryTime > exitTime || tEntry.x > 1.f || tEntry.y > 1.f || tEntry.z > 1.f) {
						std::cout << "BONJOUR";
					}
					else {
						std::vector<eDirection> collidedFaces;
						if (tEntry.y >= tEntry.z) {
							if (tEntry.y > tEntry.x) {
								if (dEntry.y > 0.0f) {
									collidedFaces.push_back(eDirection::BOTTOM);
								}
								else {
									collidedFaces.push_back(eDirection::TOP);
								}

								if (tEntry.y == tEntry.z) {
									if (dEntry.z > 0.0f) {
										collidedFaces.push_back(eDirection::FRONT);
									}
									else {
										collidedFaces.push_back(eDirection::BACK);
									}
								}
							}
							else {
								if (dEntry.x > 0.0f) {
									collidedFaces.push_back(eDirection::RIGHT);
								}
								else {
									collidedFaces.push_back(eDirection::LEFT);
								}
								if (tEntry.y == tEntry.x) {
									if (dEntry.y > 0.0f) {
										collidedFaces.push_back(eDirection::BOTTOM);
									}
									else {
										collidedFaces.push_back(eDirection::TOP);
									}
								}
							}
						}
						else {
							if (tEntry.z > tEntry.x) {
								if (dEntry.z > 0.0f) {
									collidedFaces.push_back(eDirection::FRONT);
								}
								else {
									collidedFaces.push_back(eDirection::BACK);
								}
							}
							else {
								if (dEntry.x > 0.0f) {
									collidedFaces.push_back(eDirection::RIGHT);
								}
								else {
									collidedFaces.push_back(eDirection::LEFT);
								}
								if (tEntry.z == tEntry.x) {
									if (dEntry.z > 0.0f) {
										collidedFaces.push_back(eDirection::FRONT);
									}
									else {
										collidedFaces.push_back(eDirection::BACK);
									}
								}
							}
						}

						for (auto& c : collidedFaces) {
							glm::vec2 whenWhere = (collisions.find(c))->second;

							if (c == RIGHT || c == LEFT) {
								if (whenWhere[0] > tEntry.x) {
									collisions.at(c) = { tEntry.x, dEntry.x };
								}
							}

							if (c == TOP) {
								if (whenWhere[0] > tEntry.y) {
									collisions.at(c) = { tEntry.y, dEntry.y };
								}
							}
							if (c == BOTTOM) {
								if (whenWhere[0] > tEntry.y) {
									collisions.at(c) = { tEntry.y, dEntry.y };
								}
							}

							if (c == FRONT || c == BACK) {
								if (whenWhere[0] > tEntry.z) {
									collisions.at(c) = { tEntry.z, dEntry.z };
								}
							}
						}
					}
				}
			}
		}
	
		for (auto& col : collisions) {
			//TODO réfléchir si ça touche des 2 cotés opposés
			eDirection c = col.first;
			glm::vec2 whenWhere = col.second;
			if (whenWhere[1] != INFINY) {
				if (c == RIGHT || c == LEFT) {
					move.x = whenWhere[1];
				}

				if (c == TOP) {
					move.y = whenWhere[1];
				}
				if (c == BOTTOM) {
					move.y = whenWhere[1];
					RigidBody& rb = gCoordinator.GetCompenent<RigidBody>(go);
					rb.velocity = { 0.f, 0.f, 0.f };
				}

				if (c == FRONT|| c == BACK) {
					move.z = whenWhere[1];
				}
			}
		}
		Transform& transform = gCoordinator.GetCompenent<Transform>(go);
		transform.translation += move;
	}
}
void PhysiqueSystem::calculeAABB(GameObject go){

	Mesh const mesh = gCoordinator.GetCompenent<Mesh>(go);
	AABB& aabb = gCoordinator.GetCompenent<AABB>(go);
	Transform&  transform = gCoordinator.GetCompenent<Transform>(go);
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

	glm::vec3 move{};
	if (gCoordinator.HaveComponent<RigidBody>(go)){
		RigidBody const rb = gCoordinator.GetCompenent<RigidBody>(go);
		move += rb.velocity * dt;
	}
	if (gCoordinator.HaveComponent<MotionControl>(go)) {
		MotionControl const mc = gCoordinator.GetCompenent<MotionControl>(go);
		move += mc.movement;
	}
	if (move != glm::vec3(0.f, 0.f, 0.f)) {
		AABB aabbAfterMove{};
		aabbAfterMove.max = aabb.max + move;
		aabbAfterMove.min = aabb.min + move;

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