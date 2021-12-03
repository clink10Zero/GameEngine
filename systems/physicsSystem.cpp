#include "./physicsSystem.hpp"

#include "../ecs/coordinator.hpp"
#include "../components/Transform.hpp"
#include "../components/RigidBody.hpp"
#include "../components/mesh.hpp"


#include <iostream>


extern Coordinator gCoordinator;

void PhysiqueSystem::Init()
{

}

void PhysiqueSystem::Update(float dt)
{

	for (auto const& go : mGameObject)
	{
		TestCollision(dt, go);
	}
}

void PhysiqueSystem::MakeMove(float dt, GameObject  go)
{
	if (gCoordinator.HaveComponent<RigidBody>(go))
	{
		auto& rb = gCoordinator.GetCompenent<RigidBody>(go);
		auto& transform = gCoordinator.GetCompenent<Transform>(go);


		transform.translation += rb.velocity * dt;
		rb.velocity += rb.forceGravity * dt;
	}
}

void PhysiqueSystem::TestCollision(float dt, GameObject go)
{
	//calculeAABB(go);
	//auto& aabb = gCoordinator.GetCompenent<AABB>(go);
	if (gCoordinator.HaveComponent<RigidBody>(go))
	{
		auto aabb = calculeAABBDeplacement(go, dt);

		bool move = true;
		for (auto const& go2 : mGameObject)
		{
			if (go2 != go)
			{
				//calculeAABB(go2);
				//auto& aabb2 = gCoordinator.GetCompenent<AABB>(go2);
				auto aabb2 = calculeAABBDeplacement(go2, dt);

				if ((aabb.min.x <= aabb2.min.x && aabb.max.x >= aabb2.min.x ||
					aabb.max.x >= aabb2.max.x && aabb.min.x <= aabb2.max.x ||
					aabb.min.x >= aabb2.min.x && aabb.max.x <= aabb2.max.x) &&
					(aabb.min.y <= aabb2.min.y && aabb.max.y >= aabb2.min.y ||
						aabb.max.y >= aabb2.max.y && aabb.min.y <= aabb2.max.y ||
						aabb.min.y >= aabb2.min.y && aabb.max.y <= aabb2.max.y) &&
					(aabb.min.z <= aabb2.min.z && aabb.max.z >= aabb2.min.z ||
						aabb.max.z >= aabb2.max.z && aabb.min.z <= aabb2.max.z ||
						aabb.min.z >= aabb2.min.z && aabb.max.z <= aabb2.max.z)
					) {
					//TODO gérer la collision
					move = false;
					//TODO resset gravity
				}
			}
		}
		if (move) {
			MakeMove(dt, go);
		}
	}
}

void PhysiqueSystem::calculeAABB(GameObject go)
{
	auto& aabb = gCoordinator.GetCompenent<AABB>(go);
	auto& mesh = gCoordinator.GetCompenent<Mesh>(go);
	auto& transform = gCoordinator.GetCompenent<Transform>(go);

	glm::vec3 max = mesh.data->getPostionVertex(0);
	glm::vec3 min = mesh.data->getPostionVertex(0);

	for (int i = 1; i < mesh.data->getVertexSize(); i++)
	{
		glm::vec3 tmp = mesh.data->getPostionVertex(i);

		if (max.x < tmp.x)
			max.x = tmp.x;
		if (max.y < tmp.y)
			max.y = tmp.y;
		if (max.z < tmp.z)
			max.z = tmp.z;

		if (min.x > tmp.x)
			min.x = tmp.x;
		if (min.y > tmp.y)
			min.y = tmp.y;
		if (min.z > tmp.z)
			min.z = tmp.z;
	}

	aabb.max = max * transform.scale + transform.translation;
	aabb.min = min * transform.scale + transform.translation;

}

AABB PhysiqueSystem::calculeAABBDeplacement(GameObject go, float dt)
{
	calculeAABB(go);
	auto& aabb = gCoordinator.GetCompenent<AABB>(go);

	if (gCoordinator.HaveComponent<RigidBody>(go))
	{
		auto& rb = gCoordinator.GetCompenent<RigidBody>(go);
		Transform afterMove{};
		afterMove.translation += rb.velocity * dt;

		AABB aabbAfterMove{};
		aabbAfterMove.max = aabb.max + afterMove.translation;
		aabbAfterMove.min = aabb.min + afterMove.translation;

		auto& mesh = gCoordinator.GetCompenent<Mesh>(go);
		auto& transform = gCoordinator.GetCompenent<Transform>(go);

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
