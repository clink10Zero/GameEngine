#include "./physicsSystem.hpp"

#include "../ecs/coordinator.hpp"
#include "../components/Transform.hpp"
#include "../components/RigidBody.hpp"

extern Coordinator gCoordinator;

void PhysiqueSystem::Init()
{

}

void PhysiqueSystem::Update(float dt)
{
	for (auto const& go : mGameObject)
	{
		auto& rb = gCoordinator.GetCompenent<RigidBody>(go);
		auto& transform = gCoordinator.GetCompenent<Transform>(go);

		transform.translation += rb.velocity * dt;
		rb.velocity += rb.forceGravity * dt;
	}
}