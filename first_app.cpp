#include "first_app.hpp"

#include "imgui/imgui_impl_vulkan.h"

#include "lve_camera.hpp"
#include "keyboard_movement_controller.hpp"
#include "lve_buffer.hpp"
#include "OffScreen.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>

#include "components/Transform.hpp"
#include "components/graph.hpp"
#include "components/RigidBody.hpp"
#include "components/mesh.hpp"
#include "components/aabb.hpp"

#include "systems/graphSystem.hpp"
#include "systems/physicsSystem.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/gridSystem.hpp"
#include "systems/collider_render_system.hpp"

#include "ecs/coordinator.hpp"

#include "imguiLayer.hpp"

// std
#include <array>
#include <stdexcept>
#include <cassert>
#include <chrono>


Coordinator gCoordinator;

std::shared_ptr<PhysiqueSystem> physicsSystem;

namespace lve {

    struct GlobalUbo {
        glm::mat4 projectionView{ 1.f };
        //glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
        glm::vec4 ambientLightColor = { 1.f, 1.f, 1.f, 0.02f };
        glm::vec3 lightPosition{ -1.f };
        alignas(16) glm::vec4 lightColor{ 1.f };
    };

    FirstApp::FirstApp() {
        globalPool = LveDescriptorPool::Builder(lveDevice)
            .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        loadCoordinateur();
        loadGameObject();
    }

    FirstApp::~FirstApp() { }

    void FirstApp::run() {

        std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<LveBuffer>(
                lveDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        auto globalSetLayout =
            LveDescriptorSetLayout::Builder(lveDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            LveDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);

        }

        OffScreen screen(lveDevice, globalSetLayout->getDescriptorSetLayout());

        SimpleRenderSystem simpleRenderSystem{ lveDevice, screen.GetRenderPass(), globalSetLayout->getDescriptorSetLayout() };

        //GridSystem gridsystem{ lveDevice,screen.GetRenderPass(), globalSetLayout->getDescriptorSetLayout() };
        ColliderRenderSystem colliderRenderSystem{ lveDevice, screen.GetRenderPass(), globalSetLayout->getDescriptorSetLayout() };
        ColliderRenderSystem colliderRenderSystem{ lveDevice, screen.GetRenderPass(), globalSetLayout->getDescriptorSetLayout() };

        LveCamera camera{};

        auto viewerObject = LveGameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        Imgui m_Imgui{ lveWindow, lveDevice, lveRenderer.getSwapChainRenderPass(), lveRenderer.getImageCount()};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!lveWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlanXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.getAspectRatio();

            camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 100.f);
            if (!editor)
            {
                physicsSystem->Update(frameTime);
            }

            if (auto commandBuffer = lveRenderer.beginFrame()) {
                int frameIndex = lveRenderer.getFrameIndex();
                FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex] };

                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                screen.Start(frameInfo);
                simpleRenderSystem.renderScene(frameInfo, racine);
                //gridsystem.render(frameInfo);
                colliderRenderSystem.renderScene(frameInfo, racine);
                screen.End(frameInfo);

                //render
                m_Imgui.newFrame();
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                
                //fenêtre principal
                ImGui::Begin("Game");
                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                auto size = ImGui::GetWindowSize();
                size.y -= lineHeight - 2.0f;
                
                if (ImGui::Button(playLabel.c_str(), ImVec2{ lineHeight - (size.x / 2.0f), lineHeight }))
                {
                    if (editor)
                    {
                        editor = false;
                        playLabel = "Pause";
                        //TODO copy coordinator
                        gCoordinatorSaveEditor = gCoordinator;
                    }
                    else
                    {
                        editor = true;
                        playLabel = "Play";
                        //TODO copy coordinator
                        gCoordinator = gCoordinator;
                    }
                }

                ImGui::Separator();
                ImGui::Image((ImTextureID) ImGui_ImplVulkan_AddTexture(screen.GetSampler(), screen.GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL), { size.x, (size.y - 36.0f) });
                ImGui::End();
                
                hierarchy.OnImGuiRender(racine);

                m_Imgui.render(commandBuffer);

                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();

            }
        }
        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadCoordinateur()
    {
        gCoordinator.Init();

        gCoordinator.RegisterComponent<Transform>();
        gCoordinator.RegisterComponent<RigidBody>();
        gCoordinator.RegisterComponent<Mesh>();
        gCoordinator.RegisterComponent<Graph>();
        gCoordinator.RegisterComponent<AABB>();

        physicsSystem = gCoordinator.RegisterSystem<PhysiqueSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Transform>());
            signature.set(gCoordinator.GetComponentType<AABB>());
            signature.set(gCoordinator.GetComponentType<Mesh>());

            gCoordinator.SetSystemSignature<PhysiqueSystem>(signature);
        }

        physicsSystem->Init();

        auto graphSystem = gCoordinator.RegisterSystem<GraphSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Graph>());
            gCoordinator.SetSystemSignature<GraphSystem>(signature);
        }
    }

    void FirstApp::loadGameObject() {
        racine = gCoordinator.CreateGameObject();

        Graph g_racine{};
        g_racine.parent = NULL;

        Transform t_racine{};
        t_racine.translation = glm::vec3(0.f, 0.f, 0.f);
        t_racine.rotation = glm::vec3(0.f, 0.f, 0.f);
        t_racine.scale = glm::vec3(1.f, 1.f, 1.f);

        gCoordinator.AddComponent<Transform>(racine, t_racine);

        GameObject wolf = gCoordinator.CreateGameObject();
        Mesh m_wolf;
        m_wolf.path = "models/Wolf_obj.obj";
        m_wolf.lod = 0;
        m_wolf.data = LveModel::createModelFromFile(lveDevice, m_wolf.path, m_wolf.lod);
        gCoordinator.AddComponent<Mesh>(wolf, m_wolf);

        Transform t_wolf{};

        t_wolf.translation = glm::vec3(0.f, 0.f, 0.f);
        t_wolf.rotation = glm::vec3(0.f, 0.f, 0.f);
        t_wolf.scale = glm::vec3(1.f, 1.f, 1.f);

        gCoordinator.AddComponent<Transform>(wolf, t_wolf);

        RigidBody rb_wolf{};

        rb_wolf.forceGravity = glm::vec3{ 0.f, 9.f, 0.f };

        gCoordinator.AddComponent<RigidBody>(wolf, rb_wolf);

        gCoordinator.AddComponent<AABB>(wolf, AABB{});

        Graph g_wolf{};

        GameObject floor = gCoordinator.CreateGameObject();
        Mesh m_floor{};
        m_floor.path = "models/colored_cube.obj";
        m_floor.lod = 0;
        m_floor.data = LveModel::createModelFromFile(lveDevice, m_floor.path, m_floor.lod);

        gCoordinator.AddComponent<Mesh>(floor, m_floor);

        Transform t_floor{};
        t_floor.translation = glm::vec3(0.f, 2.f, 0.f);
        t_floor.rotation = glm::vec3(0.f, 0.f, 0.f);
        t_floor.scale = glm::vec3(4.f, 0.5f, 4.f);

        gCoordinator.AddComponent<Transform>(floor, t_floor);

        gCoordinator.AddComponent<AABB>(floor, AABB{});

        Graph g_floor{};

        g_racine.children.push_back(wolf);
        g_racine.children.push_back(vase);
        g_racine.children.push_back(floor);

        gCoordinator.AddComponent<Graph>(racine, g_racine);
        g_wolf.parent = racine;
        gCoordinator.AddComponent<Graph>(wolf, g_wolf);
        g_floor.parent = racine;
        gCoordinator.AddComponent<Graph>(floor, g_floor);
        g_vase.parent = racine;
        gCoordinator.AddComponent<Graph>(vase, g_vase);
    }
}