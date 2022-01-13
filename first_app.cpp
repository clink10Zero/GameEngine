#include "first_app.hpp"

#include "imgui/imgui_impl_vulkan.h"

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
#include "components/terrain.hpp"
#include "components/Camera.hpp"
#include "components/MotionControl.hpp"
#include "components/ViewControl.hpp"

#include "systems/graphSystem.hpp"
#include "systems/physicsSystem.hpp"
#include "systems/gridSystem.hpp"
#include "systems/terrainSystem.hpp"

#include "systems/viewSystem.hpp"

#include "render_system/simple_render_system.hpp"
#include "render_system/collider_render_system.hpp"
#include "render_system/point_light_render_system.hpp"

#include "ecs/coordinator.hpp"

#include "projectSetting.hpp"

#include "imguiLayer.hpp"
#include "print.hpp"

// std
#include <array>
#include <stdexcept>
#include <cassert>
#include <chrono>

using namespace printGUI;
using namespace setting;

Coordinator gCoordinator;
std::shared_ptr<PhysiqueSystem> physicsSystem;
std::shared_ptr<TerrainSystem> terrainSystem;

std::shared_ptr<GraphSystem> graphSystem;
std::shared_ptr<ViewSystem> viewSystem;

//permet de savoir si on doit afficher ou non une window
Print affichage{};

//parametre general du moteur de jeux
ProjectSetting pSetting{};

namespace lve {

    struct GlobalUbo {
        glm::mat4 projection{ 1.f };
        glm::mat4 view{ 1.f };
        //glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
        glm::vec4 ambientLightColor = { 1.f, 1.f, 1.f, 20.f };
        glm::vec3 lightPosition{ 11.f,-12.f,5.f };
        alignas(16) glm::vec4 lightColor{ 1.f };
    };

    FirstApp::FirstApp() {
        globalPool = LveDescriptorPool::Builder(lveDevice)
            .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        loadCoordinateur(lveWindow.getGLFWwindow());
        loadGameObject();
        viewSystem->Init(lveRenderer.getAspectRatio(), lveWindow.getGLFWwindow());
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
        PointLightRenderSystem pointLightRenderSystem{ lveDevice, screen.GetRenderPass(), globalSetLayout->getDescriptorSetLayout() };
        //GridSystem gridsystem{ lveDevice,screen.GetRenderPass(), globalSetLayout->getDescriptorSetLayout() };
        //ColliderRenderSystem colliderRenderSystem{ lveDevice, screen.GetRenderPass(), globalSetLayout->getDescriptorSetLayout() };

        Imgui m_Imgui{ lveWindow, lveDevice, lveRenderer.getSwapChainRenderPass(), lveRenderer.getImageCount()};

        auto currentTime = std::chrono::high_resolution_clock::now();

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        //init system
        terrainSystem->Init();

        while (!lveWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            if (!editor)
            {
                physicsSystem->Update(frameTime);
            }
            viewSystem->Update(lveRenderer.getAspectRatio(), frameTime);

            terrainSystem->Update(frameTime, lveDevice);

            if (auto commandBuffer = lveRenderer.beginFrame()) {
                int frameIndex = lveRenderer.getFrameIndex();
                FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, globalDescriptorSets[frameIndex] };

                GlobalUbo ubo{};
                ubo.projection = viewSystem->getProjection();
                ubo.view = viewSystem->getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                screen.Start(frameInfo);
                simpleRenderSystem.renderScene(frameInfo, racine);
                pointLightRenderSystem.renderScene(frameInfo);
                //gridsystem.render(frameInfo);
                //colliderRenderSystem.renderScene(frameInfo, racine);
                screen.End(frameInfo);

                //render
                m_Imgui.newFrame();
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                
                bool open = true;
                ImGui::Begin("dockSpace", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

                ImGuiIO& io = ImGui::GetIO();
                if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                    ImGui::DockSpace(dockspace_id, ImVec2(0.f , 0.f), dockspace_flags);
                }
                ImGui::End();

                menuBarre.OnImGuiRender();

                if (affichage.playPause) {
                    ImGui::Begin("PlayPause");

                    if (ImGui::Button(playLabel.c_str(), ImVec2{ 100, 20 }))
                    {
                        if (editor)
                        {
                            editor = false;
                            playLabel = "Pause";
                            //TODO copy coordinator
                            //gCoordinatorSaveEditor = gCoordinator;
                        }
                        else
                        {
                            editor = true;
                            playLabel = "Play";
                            //TODO copy coordinator
                            //gCoordinator = gCoordinator;
                        }
                    }
                    ImGui::End();
                }

                //fenêtre principal
                if (affichage.game)
                {
                    ImGui::Begin("Game");
                    auto size = ImGui::GetWindowSize();
                    ImGui::Image((ImTextureID)ImGui_ImplVulkan_AddTexture(screen.GetSampler(), screen.GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL), { size.x, size.y });
                    ImGui::End();
                }
                //fin fenêtre principal

                hierarchy.OnImGuiRender(racine);

                io.DisplaySize = ImVec2((float)1280, (float)720);

                m_Imgui.render(commandBuffer);
                
                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    ImGui::UpdatePlatformWindows();
                    ImGui::RenderPlatformWindowsDefault();
                }

                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();

            }
        }
        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadCoordinateur(GLFWwindow* window)
    {
        gCoordinator.Init();

        gCoordinator.RegisterComponent<Transform>();
        gCoordinator.RegisterComponent<RigidBody>();
        gCoordinator.RegisterComponent<Mesh>();
        gCoordinator.RegisterComponent<Graph>();
        gCoordinator.RegisterComponent<AABB>();
        gCoordinator.RegisterComponent<Terrain>();
        gCoordinator.RegisterComponent<Chunk>();
        gCoordinator.RegisterComponent<Camera>();
        gCoordinator.RegisterComponent<MotionControl>();
        gCoordinator.RegisterComponent<ViewControl>();

        physicsSystem = gCoordinator.RegisterSystem<PhysiqueSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Transform>());
            signature.set(gCoordinator.GetComponentType<Mesh>());

            gCoordinator.SetSystemSignature<PhysiqueSystem>(signature);
        }

        physicsSystem->Init(window);

        graphSystem = gCoordinator.RegisterSystem<GraphSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Graph>());
            signature.set(gCoordinator.GetComponentType<Transform>());
            gCoordinator.SetSystemSignature<GraphSystem>(signature);
        }

        terrainSystem = gCoordinator.RegisterSystem<TerrainSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Terrain>());
            gCoordinator.SetSystemSignature<TerrainSystem>(signature);
        }

        viewSystem = gCoordinator.RegisterSystem<ViewSystem>();
        {
            Signature signature;
            signature.set(gCoordinator.GetComponentType<Camera>());
            gCoordinator.SetSystemSignature<ViewSystem>(signature);
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

        //Wolf
        /*
        GameObject wolf = gCoordinator.CreateGameObject();
        Mesh m_wolf;
        m_wolf.path = "models/Wolf_obj.obj";
        m_wolf.lod = 0;
        m_wolf.data = LveModel::createModelFromFile(lveDevice, m_wolf.path, m_wolf.lod);
        gCoordinator.AddComponent<Mesh>(wolf, m_wolf);

        Transform t_wolf{};
        t_wolf.translation = glm::vec3(0.f, 0.f, 0.f);
        t_wolf.rotation = glm::vec3(glm::pi<float>(), 0.f, 0.f);
        t_wolf.scale = glm::vec3(1.f, 1.f, 1.f);

        gCoordinator.AddComponent<Transform>(wolf, t_wolf);

        RigidBody rb_wolf{};
        rb_wolf.forceGravity = glm::vec3{ 0.f, 9.f, 0.f };
        //rb_wolf.velocity = rb_wolf.forceGravity;
        gCoordinator.AddComponent<RigidBody>(wolf, rb_wolf);

        gCoordinator.AddComponent<AABB>(wolf, AABB{});

        Graph g_wolf{};
        */
        //Floor
        GameObject floor = gCoordinator.CreateGameObject();

        Transform t_floor{};
        t_floor.translation = glm::vec3(0.f, 2.f, 0.f);
        t_floor.rotation = glm::vec3(0.f, 0.f, 0.f);
        t_floor.scale = glm::vec3(1.f, 1.f, 1.f);

        gCoordinator.AddComponent<Transform>(floor, t_floor);
        gCoordinator.AddComponent<Terrain>(floor, Terrain{});

        Graph g_floor{};
        
        //Big wall
        /*
        GameObject wall = gCoordinator.CreateGameObject();

        Mesh m_wall{};
        m_wall.path = "models/colored_cube.obj";
        m_wall.lod = 0;
        m_wall.data = LveModel::createModelFromFile(lveDevice, m_wall.path, m_wall.lod);

        gCoordinator.AddComponent<Mesh>(wall, m_wall);

        Transform t_wall{};
        t_wall.translation = glm::vec3(-2.f, 0.f, 0.f);
        t_wall.rotation = glm::vec3(0.f, 0.f, 0.f);
        t_wall.scale = glm::vec3(0.5f, 4.f, 4.f);

        gCoordinator.AddComponent<Transform>(wall, t_wall);

        gCoordinator.AddComponent<AABB>(wall, AABB{});

        Graph g_wall{};
        */

        //Player
        GameObject player = gCoordinator.CreateGameObject();
        Mesh m_player;
        m_player.path = "models/cube.obj";
        m_player.lod = 0;
        m_player.data = LveModel::createModelFromFile(lveDevice, m_player.path, m_player.lod);
        gCoordinator.AddComponent<Mesh>(player, m_player);

        Transform t_player{};
        t_player.translation = glm::vec3(9.f, -6.f, 5.f);
        t_player.scale = glm::vec3(.4f, .9f, .2f);

        gCoordinator.AddComponent<Transform>(player, t_player);

        RigidBody rb_player{};
        rb_player.forceGravity = glm::vec3{ 0.f, 9.f, 0.f };
        //rb_player.velocity = rb_player.forceGravity;
        gCoordinator.AddComponent<RigidBody>(player, rb_player);

        gCoordinator.AddComponent<AABB>(player, AABB{});

        MotionControl mc_player{};
        gCoordinator.AddComponent<MotionControl>(player, mc_player);

        Graph g_player{};

        //CameraPlayer
        GameObject cam = gCoordinator.CreateGameObject();

        Transform t_cam{};
        t_cam.translation = glm::vec3(0.f, -1.f, -1.f);
        gCoordinator.AddComponent<Transform>(cam, t_cam);

        Camera cam_cam{};
        gCoordinator.AddComponent<Camera>(cam, cam_cam);

        ViewControl vc_cam{};
        gCoordinator.AddComponent<ViewControl>(cam, vc_cam);

        Graph g_cam{};

        //CameraGlobale
        GameObject camg = gCoordinator.CreateGameObject();

        Transform t_camg{};
        t_camg.translation = glm::vec3(11.f, -10.f, -7.f);
        t_camg.rotation = glm::vec3(-glm::pi<float>()/6, 0.f, 0.f);
        gCoordinator.AddComponent<Transform>(camg, t_camg);

        Camera cam_camg{};
        gCoordinator.AddComponent<Camera>(camg, cam_camg);

        Graph g_camg{};


        //Graph
        g_player.children.push_back(cam);

        //g_racine.children.push_back(wolf);
        g_racine.children.push_back(floor);
        //g_racine.children.push_back(wall);
        g_racine.children.push_back(player);
        g_racine.children.push_back(camg);

        gCoordinator.AddComponent<Graph>(racine, g_racine);

        //g_wolf.parent = racine;
        //gCoordinator.AddComponent<Graph>(wolf, g_wolf);

        g_floor.parent = racine;
        gCoordinator.AddComponent<Graph>(floor, g_floor);
        
        //g_wall.parent = racine;
        //gCoordinator.AddComponent<Graph>(wall, g_wall);

        g_player.parent = racine;
        gCoordinator.AddComponent<Graph>(player, g_player);

        g_camg.parent = racine;
        gCoordinator.AddComponent<Graph>(camg, g_camg);
        
        g_cam.parent = player;
        gCoordinator.AddComponent<Graph>(cam, g_cam);
    }
}