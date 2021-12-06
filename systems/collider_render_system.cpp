#include "collider_render_system.hpp"
#include "../ecs/coordinator.hpp"
#include "./graphSystem.hpp"

#include "../components/Transform.hpp"
#include "../components/graph.hpp"
#include "../components/mesh.hpp"
#include "../components/aabb.hpp"

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

    ColliderRenderSystem::ColliderRenderSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : lveDevice{ device } {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    ColliderRenderSystem::~ColliderRenderSystem() { vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr); }


    void ColliderRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
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

    void ColliderRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannnot create pipelinebefore pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig, VK_POLYGON_MODE_LINE);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig, true);
    }

    void ColliderRenderSystem::renderScene(FrameInfo& frameInfo, GameObject racine) {
        lvePipeline->bind(frameInfo.commandBuffer);
        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

        Transform t = gCoordinator.GetCompenent<Transform>(racine);
        Graph g = gCoordinator.GetCompenent<Graph>(racine);

        for (auto& obj : g.children) {
            if (gCoordinator.HaveComponent<AABB>(obj)) {
                renderCollider(frameInfo, obj, t);
            }
            //TODO est-ce qu'un objet sans AABB peut avoir un fils qui en a un :thinking:
        }
    }

    void ColliderRenderSystem::renderCollider(FrameInfo& frameInfo, GameObject gameObject, Transform parentTransform) {
        Transform t = gCoordinator.GetCompenent<Transform>(gameObject);
        auto worldTransform = graphSystem.combineWith(t, parentTransform);
        Mesh m{};
        m.data = createAABBModel(gCoordinator.GetCompenent<AABB>(gameObject));

        SimplePushConstantData push{};
        push.modelMatrix = glm::mat4{ 1.f };
        push.normalMatrix = glm::mat4{ 1.f };

        vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
        m.data->bind(frameInfo.commandBuffer);
        m.data->draw(frameInfo.commandBuffer);

        Graph g = gCoordinator.GetCompenent<Graph>(gameObject);
        for (auto& obj : g.children) {
            renderCollider(frameInfo, obj, t);
            //TODO un objet qui a un collider a un enfant qui a un collider ? :thinking:
        }
    }

    
    std::unique_ptr<LveModel>  ColliderRenderSystem::createAABBModel(AABB aabb)
    {
        LveModel::Builder modelBuilder{};
        glm::vec3 max = aabb.max;
        glm::vec3 min = aabb.min;
        modelBuilder.vertices = {
            // left face (white)
            {{min.x, min.y, min.z}, {1.f, 1.f, 1.f}},
            {{min.x, max.y, max.z},  {1.f, 1.f, 1.f}},
            {{min.x, min.y, max.z},  {1.f, 1.f, 1.f}},
            {{min.x, max.y, min.z},  {1.f, 1.f, 1.f}},

            // right face (yellow)
            {{max.x, min.y, min.z}, {1.f, 1.f, 1.f}},
            {{max.x, max.y, max.z},  {1.f, 1.f, 1.f}},
            {{max.x, min.y, max.z},  {1.f, 1.f, 1.f}},
            {{max.x, max.y, min.z},  {1.f, 1.f, 1.f}},

            // top face (orange, remember y axis points down)
            {{min.x, min.y, min.z},  {1.f, 1.f, 1.f}},
            {{max.x, min.y, max.z},  {1.f, 1.f, 1.f}},
            {{min.x, min.y, max.z},  {1.f, 1.f, 1.f}},
            {{max.x, min.y, min.z},  {1.f, 1.f, 1.f}},

            // bottom face (red)
            {{min.x, max.y, min.z},  {1.f, 1.f, 1.f}},
            {{max.x, max.y, max.z},  {1.f, 1.f, 1.f}},
            {{min.x, max.y, max.z},  {1.f, 1.f, 1.f}},
            {{max.x, max.y, min.z},  {1.f, 1.f, 1.f}},

            // nose face (blue)
            {{min.x, min.y, max.z},  {1.f, 1.f, 1.f}},
            {{max.x, max.y, max.z},  {1.f, 1.f, 1.f}},
            {{min.x, max.y, max.z},  {1.f, 1.f, 1.f}},
            {{max.x, min.y, max.z}, {1.f, 1.f, 1.f}},

            // tail face (green)
            {{min.x, min.y, min.z},  {1.f, 1.f, 1.f}},
            {{max.x, max.y, min.z},  {1.f, 1.f, 1.f}},
            {{min.x, max.y, min.z},  {1.f, 1.f, 1.f}},
            {{max.x, min.y, min.z},  {1.f, 1.f, 1.f}},
        };

        modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

        return std::make_unique<LveModel>(lveDevice, modelBuilder);
    }
}