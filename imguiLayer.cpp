#include "imguiLayer.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

#include "lve_window.hpp"

#include <stdexcept>

namespace lve {

// ok this just initializes imgui using the provided integration files. So in our case we need to
// initialize the vulkan and glfw imgui implementations, since that's what our engine is built
// using.
    Imgui::Imgui(
            LveWindow &window, LveDevice &device, VkRenderPass renderPass, uint32_t imageCount) : lveDevice{device} {
        // set up a descriptor pool stored on this instance, see header for more comments on this.
        VkDescriptorPoolSize pool_sizes[] = {
                {VK_DESCRIPTOR_TYPE_SAMPLER, 2000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 2000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 2000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 2000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 2000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 2000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 2000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 2000}};
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 2000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        if (vkCreateDescriptorPool(device.device(), &pool_info, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up descriptor pool for imgui");
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        // Initialize imgui for vulkan
        ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = device.getInstance();
        init_info.PhysicalDevice = device.getPhysicalDevice();
        init_info.Device = device.device();
        init_info.QueueFamily = device.getGraphicsQueueFamily();
        init_info.Queue = device.graphicsQueue();

        // pipeline cache is a potential future optimization, ignoring for now
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = descriptorPool;
        // todo, I should probably get around to integrating a memory allocator library such as Vulkan
        // memory allocator (VMA) sooner than later. We don't want to have to update adding an allocator
        // in a ton of locations.
        init_info.Allocator = VK_NULL_HANDLE;
        init_info.MinImageCount = 2;
        init_info.ImageCount = imageCount;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, renderPass);

        // upload fonts, this is done by recording and submitting a one time use command buffer
        // which can be done easily bye using some existing helper functions on the lve device object
        auto commandBuffer = device.beginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        device.endSingleTimeCommands(commandBuffer);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    Imgui::~Imgui() {
        vkDestroyDescriptorPool(lveDevice.device(), descriptorPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Imgui::newFrame() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Imgui::render(VkCommandBuffer commandBuffer) {
        ImGui::Render();
        ImDrawData *drawdata = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(drawdata, commandBuffer);
    }
}