#pragma once

#include "../lve_device.hpp"
#include "../lve_pipeline.hpp"
#include "../lve_swap_chain.hpp"
#include "../lve_camera.hpp"
#include "../lve_frame_info.hpp"
#include "../lve_model.hpp"

#include "../systems/system.hpp"

#include "../components/Transform.hpp"
#include "../components/AABB.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
	class ColliderRenderSystem: public System {
		public:

			ColliderRenderSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
			~ColliderRenderSystem();

			ColliderRenderSystem(const ColliderRenderSystem&) = delete;
			ColliderRenderSystem& operator=(const ColliderRenderSystem&) = delete;

			void renderScene(FrameInfo& frameInfo, GameObject racine);
			void renderCollider(FrameInfo& frameInfo, GameObject gameObject, Transform parent);

			std::unique_ptr<LveModel> createAABBModel(AABB aabb);

		private:
			void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
			void createPipeline(VkRenderPass renderPass);

			LveDevice& lveDevice;
			std::unique_ptr<LvePipeline> lvePipeline;
			VkPipelineLayout pipelineLayout;
	};
}