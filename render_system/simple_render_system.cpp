#include "./simple_render_system.hpp"

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

    struct SimplePushConstantData {
        glm::mat4 modelMatrix{ 1.f };
        glm::mat4 normalMatrix{ 1.f };
    };

    SimpleRenderSystem::SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : lveDevice{ device } {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() { vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr); }


    void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstatnRange{};
        pushConstatnRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstatnRange.offset = 0;
        pushConstatnRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstatnRange;
        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannnot create pipelinebefore pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig, true);
    }

    void SimpleRenderSystem::renderScene(FrameInfo& frameInfo, GameObject racine) {
        lvePipeline->bind(frameInfo.commandBuffer);
        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

        Transform t = gCoordinator.GetCompenent<Transform>(racine);
        Graph g = gCoordinator.GetCompenent<Graph>(racine);

        for (auto& obj : g.children) {
            renderObject(frameInfo, obj, t);
        }
    }

    void SimpleRenderSystem::renderObject(FrameInfo& frameInfo, GameObject go, Transform parentTransform) {
        Transform t = gCoordinator.GetCompenent<Transform>(go);
        Transform worldTransform = t.combineWith(parentTransform);
        if (gCoordinator.HaveComponent<Mesh>(go)) {
            Mesh m = gCoordinator.GetCompenent<Mesh>(go);

            SimplePushConstantData push{};
            push.modelMatrix = worldTransform.mat4();
            push.normalMatrix = worldTransform.normalMatrix();

            vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
            m.data->bind(frameInfo.commandBuffer);
            m.data->draw(frameInfo.commandBuffer);
        }
        Graph g = gCoordinator.GetCompenent<Graph>(go);
        for (auto& obj : g.children) {
            renderObject(frameInfo, obj, worldTransform);
        }
    }
}