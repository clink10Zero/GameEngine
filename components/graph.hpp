#pragma once
#include "../ecs/types.hpp"

#include <vector>

struct Graph
{
	GameObject parent;
	std::vector<GameObject> children;
};