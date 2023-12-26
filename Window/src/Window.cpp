#include "Window.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <glad/glad.h>

namespace RT
{

    void Window::Init(const WindowSpecs& specs)
    {
        if (!glfwInit())
            return;

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

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            return;

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

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 430 core");

        io.Fonts->Build();
    }

    void Window::ShutDown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool Window::Update()
    {
        glfwPollEvents();
        glfwSwapBuffers(window);

        return !glfwWindowShouldClose(window);
    }

    bool Window::PullEvents()
    {
        WindowResize();
        return true;
    }

    void Window::BeginUI()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    }

    void Window::EndUI()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    glm::vec2 Window::GetMousePos() const
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return { (float)x, (float)y };
    }

    bool Window::IsKeyPressed(int32_t key) const
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    bool Window::IsMousePressed(int32_t key) const
    {
        return glfwGetMouseButton(window, key) == GLFW_PRESS;
    }

    glm::ivec2 Window::GetSize() const
    {
        int32_t width, height;
        glfwGetWindowSize(window, &width, &height);
        return { width, height };
    }

    void Window::CursorMode(int32_t state) const
    {
        glfwSetInputMode(window, GLFW_CURSOR, state);
    }

    void Window::WindowResize()
    {
        glm::ivec2 winSize = GetSize();
        if (winSize.x != width || winSize.y != height)
        {
            width = winSize.x;
            height = winSize.y;
            glViewport(0, 0, width, height);
        }
    }

}
