#pragma once

#include "lve_model.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace lve {
	/*
	struct TransformComponent{
		glm::vec3 translation{ 0.f, 0.f, 0.f };
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{ 0.f, 0.f, 0.f };

		glm::mat4 mat4();
		glm::mat3 normalMatrix();
		glm::mat3 normalWorldMatrix(TransformComponent worldPlace);

		void doAnimation(TransformComponent t, float time);
		TransformComponent combineWith(TransformComponent t);
		TransformComponent interpolate_with(TransformComponent t, float time);
		TransformComponent doRotation(int dir, float angle);
	};

	class LveGameObject{
		public:
			using id_t = unsigned int;

			static LveGameObject createGameObject() {
				static id_t currentId = 0;
				return LveGameObject(currentId++);
			}

			LveGameObject(const LveGameObject&) = delete;
			LveGameObject& operator=(const LveGameObject&) = delete;
			LveGameObject(LveGameObject&&) = default;
			LveGameObject& operator=(LveGameObject&&) = default;

			id_t getId() { return id; }

			bool hasModel = true;
			std::shared_ptr<LveModel> model{};
			glm::vec3 color{};
			
			TransformComponent transform{};
			TransformComponent anim{};
			std::vector<LveGameObject> children;

		private:
			id_t id;
			LveGameObject(id_t objId) : id{ objId } {}

	};
	*/
}