#pragma once

#include "../ecs/types.hpp"
#include <set>


class System
{
public:
	std::set<GameObject> mGameObject;
};