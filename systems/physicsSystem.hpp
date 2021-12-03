#pragma once

#include "system.hpp"
#include "../components/aabb.hpp"

class PhysiqueSystem : public System
{
	public:
		void Init();

		void Update(float dt);

		void MakeMove(float dt, GameObject go);
		void TestCollision(float dt, GameObject go);
		void calculeAABB(GameObject go);
		AABB calculeAABBDeplacement(GameObject go, float dt);
};
