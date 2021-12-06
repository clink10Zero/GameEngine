#pragma once

#include "lve_device.hpp"
#include "lve_pipeline.hpp"
#include "lve_frame_info.hpp"

#include <vulkan/vulkan.h>
#include <memory>

namespace lve {
    struct FrameBufferAttachment {
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    };
    struct OffscreenPass {
        uint32_t width, height;
        VkFramebuffer frameBuffer;
        FrameBufferAttachment color, depth;
        VkRenderPass renderPass;
        VkSampler sampler;
        VkDescriptorImageInfo descriptor;
    };

    class OffScreen {
        public:
            OffScreen(LveDevice& device, VkDescriptorSetLayout globalSetLayout);
            
            ~OffScreen()
            {
                vkDestroyImageView(m_Device.device(), pass.color.view, nullptr);
                vkDestroyImage(m_Device.device(), pass.color.image, nullptr);
                vkFreeMemory(m_Device.device(), pass.color.mem, nullptr);

                // Depth attachment
                vkDestroyImageView(m_Device.device(), pass.depth.view, nullptr);
                vkDestroyImage(m_Device.device(), pass.depth.image, nullptr);
                vkFreeMemory(m_Device.device(), pass.depth.mem, nullptr);

                vkDestroyRenderPass(m_Device.device(), pass.renderPass, nullptr);
                vkDestroySampler(m_Device.device(), pass.sampler, nullptr);
                vkDestroyFramebuffer(m_Device.device(), pass.frameBuffer, nullptr);
            }

            VkSampler GetSampler() { return pass.sampler; }
            VkImageView GetImageView() { return pass.color.view; }
            VkRenderPass GetRenderPass() { return pass.renderPass; }

            void SetViewportSize(const glm::vec2& size) { pass.width = static_cast<uint32_t>(size.x); pass.height = static_cast<uint32_t>(size.y); }

            void Start(FrameInfo frameInfo);
            void End(FrameInfo frameInfo);

        private:
            void setupPipelineLayout(VkDescriptorSetLayout globalSetLayout);
            void preparePipelines();

            OffscreenPass pass;
            LveDevice& m_Device;
            VkPipelineLayout pipelineLayout;
            std::unique_ptr<LvePipeline> m_Pipeline;
            VkDescriptorSetLayout DescriptorSetLayout;
    };
}