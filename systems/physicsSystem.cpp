#include "./physicsSystem.hpp"

#include "../ecs/coordinator.hpp"
#include "../components/Transform.hpp"
#include "../components/RigidBody.hpp"
#include "../components/mesh.hpp"
#include "../components/aabb.hpp"

extern Coordinator gCoordinator;

void PhysiqueSystem::Init()
{

}

void PhysiqueSystem::Update(float dt)
{
	for (auto const& go : mGameObject)
	{
		MakeMove(dt, go);
	}
}
void PhysiqueSystem::MakeMove(float dt, GameObject  go)
{
	auto& rb = gCoordinator.GetCompenent<RigidBody>(go);
	auto& transform = gCoordinator.GetCompenent<Transform>(go);

	transform.translation += rb.velocity * dt;
	rb.velocity += rb.forceGravity * dt;

}

void PhysiqueSystem::TestCollision(float dt, GameObject go)
{
	calculeAABB(go);
	auto& aabb = gCoordinator.GetCompenent<AABB>(go);

	glm::vec3 dimension = aabb.max - aabb.min;
	bool move = true;
	for (auto const& go2 : mGameObject)
	{
		calculeAABB(go2);
		auto& aabb2 = gCoordinator.GetCompenent<AABB>(go2);

		if (go2 != go)
		{
			if ((aabb2.min.x >= aabb.min.x + dimension.x)
				|| (aabb2.min.x + dimension.x >= aabb.min.x)
				|| (aabb2.min.y >= aabb.min.y + dimension.y)
				|| (aabb2.min.y + dimension.y >= aabb.min.y)
				|| (aabb2.min.z >= aabb.min.z + dimension.z)
				|| (aabb2.min.z + dimension.z >= aabb.min.z)
				)
			{
				move = false;
			}
		}
	}

	if (move)
		MakeMove(dt, go);
}
void PhysiqueSystem::calculeAABB(GameObject go)
{
	auto& aabb = gCoordinator.GetCompenent<AABB>(go);
	auto& mesh = gCoordinator.GetCompenent<Mesh>(go);
	auto& transform = gCoordinator.GetCompenent<Transform>(go);

	glm::vec3 max = mesh->getPostionVertex(0);
	glm::vec3 min = mesh->getPostionVertex(0);

	for (int i = 1; i < mesh->getVertexSize(); i++)
	{
		glm::vec3 tmp = mesh->getPostionVertex(i);

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

	aabb.max = (max * transform.scale) + transform.translation;
	aabb.min = (min * transform.scale) + transform.translation;
}