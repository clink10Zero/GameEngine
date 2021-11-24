#include "./graphSystem.hpp"
#include "../ecs/coordinator.hpp"


glm::mat4 GraphSystem::mat4(Transform self)
{
	const float c3 = glm::cos(self.rotation.z);
	const float s3 = glm::sin(self.rotation.z);
	const float c2 = glm::cos(self.rotation.x);
	const float s2 = glm::sin(self.rotation.x);
	const float c1 = glm::cos(self.rotation.y);
	const float s1 = glm::sin(self.rotation.y);
	return glm::mat4{
		{
			self.scale.x * (c1 * c3 + s1 * s2 * s3),
			self.scale.x * (c2 * s3),
			self.scale.x * (c1 * s2 * s3 - c3 * s1),
			0.0f,
		},
		{
			self.scale.y * (c3 * s1 * s2 - c1 * s3),
			self.scale.y * (c2 * c3),
			self.scale.y * (c1 * c3 * s2 + s1 * s3),
			0.0f,
		},
		{
			self.scale.z * (c2 * s1),
			self.scale.z * (-s2),
			self.scale.z * (c1 * c2),
			0.0f,
		},
		{self.translation.x, self.translation.y, self.translation.z, 1.0f} };
}
glm::mat3 GraphSystem::normalMatrix(Transform self)
{
	const float c3 = glm::cos(self.rotation.z);
	const float s3 = glm::sin(self.rotation.z);
	const float c2 = glm::cos(self.rotation.x);
	const float s2 = glm::sin(self.rotation.x);
	const float c1 = glm::cos(self.rotation.y);
	const float s1 = glm::sin(self.rotation.y);
	const glm::vec3 invScale = 1.0f / self.scale;

	return glm::mat3{
		{
			invScale.x * (c1 * c3 + s1 * s2 * s3),
			invScale.x * (c2 * s3),
			invScale.x * (c1 * s2 * s3 - c3 * s1),
		},
		{
			invScale.y * (c3 * s1 * s2 - c1 * s3),
			invScale.y * (c2 * c3),
			invScale.y * (c1 * c3 * s2 + s1 * s3),
		},
		{
			invScale.z * (c2 * s1),
			invScale.z * (-s2),
			invScale.z * (c1 * c2),
		},
	};
}

glm::mat3 GraphSystem::normalWorldMatrix(Transform worldPlace)
{
	const float c3 = glm::cos(worldPlace.rotation.z);
	const float s3 = glm::sin(worldPlace.rotation.z);
	const float c2 = glm::cos(worldPlace.rotation.x);
	const float s2 = glm::sin(worldPlace.rotation.x);
	const float c1 = glm::cos(worldPlace.rotation.y);
	const float s1 = glm::sin(worldPlace.rotation.y);
	const glm::vec3 invScale = 1.0f / worldPlace.scale;

	return glm::mat3{
		{
			invScale.x * (c1 * c3 + s1 * s2 * s3),
			invScale.x * (c2 * s3),
			invScale.x * (c1 * s2 * s3 - c3 * s1),
		},
		{
			invScale.y * (c3 * s1 * s2 - c1 * s3),
			invScale.y * (c2 * c3),
			invScale.y * (c1 * c3 * s2 + s1 * s3),
		},
		{
			invScale.z * (c2 * s1),
			invScale.z * (-s2),
			invScale.z * (c1 * c2),
		},
	};
}

Transform GraphSystem::combineWith(Transform self, Transform t)
{
	Transform result{};
	result.translation = self.translation + t.translation;
	result.rotation = self.rotation + t.rotation;
	result.scale = self.scale * t.scale;
	return result;
}
Transform GraphSystem::interpolate_with(Transform self, Transform t, float time)
{
	Transform result{};
	result.scale = self.scale;
	result.rotation = self.rotation + (t.rotation * time);
	result.translation = self.translation + (t.translation * time);
	return result;
}