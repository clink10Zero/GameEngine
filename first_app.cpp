#include "first_app.hpp"

#include "lve_camera.hpp"
#include "keyboard_movement_controller.hpp"
#include "lve_buffer.hpp"

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

#include "ecs/coordinator.hpp"

// std
#include <array>
#include <stdexcept>
#include <cassert>
#include <chrono>

#include <iostream>

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

        SimpleRenderSystem simpleRenderSystem{ lveDevice,lveRenderer.getSwapChainRenderPass(),globalSetLayout->getDescriptorSetLayout() };
        LveCamera camera{};

        auto viewerObject = LveGameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

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

            physicsSystem->Update(frameTime);

            if (auto commandBuffer = lveRenderer.beginFrame()) {
                int frameIndex = lveRenderer.getFrameIndex();
                FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex] };

                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderScene(frameInfo, racine);
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
            signature.set(gCoordinator.GetComponentType<RigidBody>());
            signature.set(gCoordinator.GetComponentType<AABB>());
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

        GameObject cube = gCoordinator.CreateGameObject();
        gCoordinator.AddComponent<Mesh>(cube, (Mesh)LveModel::createModelFromFile(lveDevice, "models/colored_cube.obj"));

        Transform t_cube{};
        t_cube.translation = glm::vec3(0.f, 0.f, 0.f);
        t_cube.rotation = glm::vec3(0.f, 0.f, 0.f);
        t_cube.scale = glm::vec3(.5f, .5f, .5f);

        gCoordinator.AddComponent<Transform>(cube, t_cube);

        RigidBody rb_cube{};

        rb_cube.forceGravity = glm::vec3{ 0.f, 9.f, 0.f };

        gCoordinator.AddComponent<RigidBody>(cube, rb_cube);

        gCoordinator.AddComponent<AABB>(cube, AABB{});

        Graph g_cube{};

        GameObject floor = gCoordinator.CreateGameObject();
        gCoordinator.AddComponent<Mesh>(floor, (Mesh)LveModel::createModelFromFile(lveDevice, "models/colored_cube.obj"));

        Transform t_floor{};
        t_floor.translation = glm::vec3(0.f, 2.f, 0.f);
        t_floor.rotation = glm::vec3(0.f, 0.f, 0.f);
        t_floor.scale = glm::vec3(4.f, 0.5f, 4.f);

        gCoordinator.AddComponent<Transform>(floor, t_floor);

        gCoordinator.AddComponent<AABB>(floor, AABB{});

        Graph g_floor{};

        g_racine.children.push_back(cube);
        g_racine.children.push_back(floor);
        gCoordinator.AddComponent<Graph>(racine, g_racine);
        g_cube.parent = racine;
        gCoordinator.AddComponent<Graph>(cube, g_cube);
        g_floor.parent = racine;
        gCoordinator.AddComponent<Graph>(floor, g_floor);
    }
}