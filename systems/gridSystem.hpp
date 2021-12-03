#pragma once

#include "../lve_camera.hpp"
#include "../lve_device.hpp"
#include "../lve_frame_info.hpp"
#include "../lve_pipeline.hpp"


#include <memory>
#include <vector>

namespace lve {
	class GridSystem {

		public:
			GridSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
			~GridSystem();

			GridSystem(const GridSystem&) = delete;
			GridSystem& operator=(const GridSystem&) = delete;

			void render(FrameInfo& frameInfo);
		private:
			void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
			void createPipeline(VkRenderPass renderPass);

			LveDevice& m_Device;

			std::unique_ptr<LvePipeline> m_Pipeline;
			VkPipelineLayout pipelineLayout;
	};
}