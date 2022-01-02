#include "./point_light_render_system.hpp"

#include "../ecs/coordinator.hpp"

#include "../systems/graphSystem.hpp"

#include "../components/Transform.hpp"
#include "../components/graph.hpp"
#include "../components/mesh.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>

// std
#include <array>
#include <stdexcept>
#include <cassert>

#include <iostream>

extern Coordinator gCoordinator;
extern GraphSystem graphSystem;

namespace lve {

    PointLightRenderSystem::PointLightRenderSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : lveDevice{ device } {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    PointLightRenderSystem::~PointLightRenderSystem() { vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr); }


    void PointLightRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        /*VkPushConstantRange pushConstatnRange{};
        pushConstatnRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstatnRange.offset = 0;
        pushConstatnRange.size = sizeof(SimplePushConstantData);*/

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void PointLightRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannnot create pipelinebefore pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig, VK_POLYGON_MODE_FILL);
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/point_light_shader.vert.spv",
            "shaders/point_light_shader.frag.spv",
            pipelineConfig, true);
    }

    void PointLightRenderSystem::renderScene(FrameInfo& frameInfo) {
        lvePipeline->bind(frameInfo.commandBuffer);
        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);
        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
}
