#pragma once

#include "lve_device.hpp"
#include "lve_window.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imstb_rectpack.h"
#include "imgui/imstb_textedit.h"
#include "imgui/imstb_truetype.h"

namespace lve {

	static void check_vk_result(VkResult err) {
		if (err == 0) return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0) abort();
	}

	class Imgui {
		public :
			Imgui(LveWindow& window, LveDevice& device, VkRenderPass renderPass, uint32_t imageCount);
			~Imgui();

			void newFrame();

			void render(VkCommandBuffer commandBuffer);

		private:
			LveDevice &lveDevice;

			VkDescriptorPool descriptorPool;
	};
}