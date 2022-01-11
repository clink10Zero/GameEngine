#pragma once

#include "./system.hpp"
#include "../components/Transform.hpp"

class GraphSystem : public System
{
	public:
		Transform getWorldPosition(GameObject go);


};
