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
	class PointLightRenderSystem: public System {
		public:

			PointLightRenderSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
			~PointLightRenderSystem();

			PointLightRenderSystem(const PointLightRenderSystem&) = delete;
			PointLightRenderSystem& operator=(const PointLightRenderSystem&) = delete;

			void renderScene(FrameInfo& frameInfo);

		private:
			void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
			void createPipeline(VkRenderPass renderPass);

			LveDevice& lveDevice;
			std::unique_ptr<LvePipeline> lvePipeline;
			VkPipelineLayout pipelineLayout;
	};
}