#pragma once

#include "./system.hpp"
#include "../components/Transform.hpp"

class GraphSystem : public System
{
public:

	static glm::mat4 mat4(Transform self);
	static glm::mat3 normalMatrix(Transform self);
	static glm::mat3 normalWorldMatrix(Transform worldPlace);

	static Transform combineWith(Transform self, Transform t);
	static Transform interpolate_with(Transform self, Transform t, float time);

};
