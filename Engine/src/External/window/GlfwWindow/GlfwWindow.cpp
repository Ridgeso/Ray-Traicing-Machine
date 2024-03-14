#include "GlfwWindow.h"
#include <vector>
#include "Engine/Core/Assert.h"

#include <imgui.h>
//#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>
#include <glad/glad.h>

#include "Engine/Render/RenderBase.h"

namespace RT
{

    void GlfwWindow::init(const WindowSpecs& specs)
    {
        if (!glfwInit())
        {
            return;
        }
        if (GlobalRenderAPI == RenderAPI::Vulkan)
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        }

        title = specs.titel;
        width = specs.width;
        height = specs.height;
        isMinimized = specs.isMinimized;

        window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        // initImGui();
    }

    void GlfwWindow::shutDown()
    {
        //ImGui_ImplOpenGL3_Shutdown();
        
        //ImGui_ImplVulkan_Shutdown();
        //ImGui_ImplGlfw_Shutdown();
        //ImGui::DestroyContext();
        //ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
        
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool GlfwWindow::update()
    {
        glfwPollEvents();
        glfwSwapBuffers(window);

        return !glfwWindowShouldClose(window);
    }

    bool GlfwWindow::pullEvents()
    {
        return true;
    }

    void GlfwWindow::beginUI()
    {
        //ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    }

    void GlfwWindow::endUI()
    {
        ImGui::Render();
        //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuff, pipline);
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    glm::vec2 GlfwWindow::getMousePos() const
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return { (float)x, (float)y };
    }

    bool GlfwWindow::isKeyPressed(int32_t key) const
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    bool GlfwWindow::isMousePressed(int32_t key) const
    {
        return glfwGetMouseButton(window, key) == GLFW_PRESS;
    }

    glm::ivec2 GlfwWindow::getSize() const
    {
        int32_t width, height;
        glfwGetWindowSize(window, &width, &height);
        return { width, height };
    }

    void GlfwWindow::cursorMode(int32_t state) const
    {
        glfwSetInputMode(window, GLFW_CURSOR, state);
    }

    void GlfwWindow::initImGui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard
            | ImGuiConfigFlags_DockingEnable
            | ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        //ImGui_ImplGlfw_InitForOpenGL(window, true);
        //ImGui_ImplOpenGL3_Init("#version 430 core");
        ImGui_ImplGlfw_InitForVulkan(window, true);

        //auto vkWindowH = ImGui_ImplVulkanH_Window();
        //vkWindowH.Surface = surface;
        //
        //// Check for WSI support
        //VkBool32 res;
        //vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, vkWindowH.Surface, &res);
        //RT_CORE_ASSERT(res == VK_TRUE, "Error no WSI support on physical device 0");
        //
        //// Select Surface Format
        //const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
        //const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        //vkWindowH.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, vkWindowH.Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);
        //
        //std::vector<VkPresentModeKHR> present_modes{};
        //if (presentMode)
        //{
        //    present_modes.push_back(VK_PRESENT_MODE_MAILBOX_KHR);
        //    present_modes.push_back(VK_PRESENT_MODE_IMMEDIATE_KHR);
        //    present_modes.push_back(VK_PRESENT_MODE_FIFO_KHR);
        //}
        //else
        //{
        //   present_modes.push_back(VK_PRESENT_MODE_FIFO_KHR);
        //}
        //
        //vkWindowH.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, vkWindowH.Surface, present_modes.data(), IM_ARRAYSIZE(present_modes));
        //RT_LOG_INFO("[vulkan] Selected PresentMode = {}", vkWindowH.PresentMode);
        //
        //// Create SwapChain, RenderPass, Framebuffer, etc.
        //RT_ASSERT(g_MinImageCount >= 2);
        //ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &vkWindowH, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
        //
        //auto vkInfo = ImGui_ImplVulkan_InitInfo();
        //vkInfo.Instance = instance;
        //vkInfo.PhysicalDevice = physicalDevice;
        //vkInfo.Device = device;
        //vkInfo.QueueFamily = queueFamily;
        //vkInfo.Queue = queue;
        //vkInfo.PipelineCache = pipelineCache;
        //vkInfo.DescriptorPool = descriptorPool;
        //vkInfo.Subpass = 0;
        //vkInfo.MinImageCount = minImageCount;
        //vkInfo.ImageCount = wd->ImageCount;
        //vkInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        //vkInfo.Allocator = nullptr;
        //vkInfo.CheckVkResultFn = checkVkResultCallback;
        //ImGui_ImplVulkan_Init(&vkInfo, renderpass);

        io.Fonts->Build();
        ImGui::StyleColorsClassic();
    }

}
