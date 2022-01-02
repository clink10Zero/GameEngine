#pragma once

#include "../lve_device.hpp"
#include "../lve_pipeline.hpp"
#include "../lve_swap_chain.hpp"
#include "../lve_camera.hpp"
#include "../lve_frame_info.hpp"

#include "../systems/system.hpp"

#include "../components/Transform.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
	class SimpleRenderSystem: public System {
		public:

			SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
			~SimpleRenderSystem();

			SimpleRenderSystem(const SimpleRenderSystem&) = delete;
			SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

			void renderScene(FrameInfo& frameInfo, GameObject racine);
			void renderObject(FrameInfo& frameInfo, GameObject gameObject, Transform parent);

		private:
			void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
			void createPipeline(VkRenderPass renderPass);

			LveDevice& lveDevice;
			std::unique_ptr<LvePipeline> lvePipeline;
			VkPipelineLayout pipelineLayout;
	};
}