#include "lve_game_object.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
namespace lve {
	glm::mat4 lve::TransformComponent::mat4() {
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		return glm::mat4{
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f,
			},
			{translation.x, translation.y, translation.z, 1.0f} };
	}

	
	glm::mat3 TransformComponent::normalMatrix() {
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 invScale = 1.0f / scale;

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

	glm::mat3 TransformComponent::normalWorldMatrix(TransformComponent worldPlace) {
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
	
	void TransformComponent::doAnimation(TransformComponent t, float time) {
		TransformComponent a = this->interpolate_with(t, time);
		scale = a.scale;
		rotation = a.rotation;
		translation = a.translation;
	}

	TransformComponent TransformComponent::combineWith(TransformComponent t) {
		TransformComponent result{};
		result.translation = translation + t.translation;
		result.rotation = rotation + t.rotation;
		result.scale = scale * t.scale;
		return result;
	}

	TransformComponent TransformComponent::interpolate_with(TransformComponent t, float time) {
		TransformComponent result{};
		result.scale = this->scale;
		result.rotation = this->rotation + (t.rotation * time);
		result.translation = this->translation + (t.translation * time);
		return result;
	}

	TransformComponent TransformComponent::doRotation(int dir, float angle) {
		TransformComponent result{};

		float yaw = rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), .0f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, .0f, -forwardDir.x };
		const glm::vec3 upDir{ .0f, -1.f, .0f };

		glm::vec3 axe;

		switch (dir)
		{
		case 0: //rotation sur l'axe fowardDir -> balancier gauche/droite
			axe = forwardDir;
			break;
		case 1: //rotation sur l'axe rightDir -> balancier avant/arrière
			axe = rightDir;
			break;
		default://rotation sur l'axe upDir -> toupie
			axe = upDir;
			break;
		}

		glm::quat MyQuaternion;
		MyQuaternion = glm::angleAxis(glm::degrees(angle), axe);


		return result;
	}
}
