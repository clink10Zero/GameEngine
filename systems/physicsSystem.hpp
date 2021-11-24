#pragma once

#include "system.hpp"

class PhysiqueSystem : public System
{
	public:
		void Init();

		void Update(float dt);

		void MakeMove(float dt, GameObject go);
		void TestCollision(float dt, GameObject go);
		void calculeAABB(GameObject go);
};
