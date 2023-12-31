#include "Window.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <glad/glad.h>

namespace RT::Instance
{
	
	bool Window::Init()
	{
        if (!glfwInit())
            return false;

        m_Width = 1280;
        m_Height = 720;
        m_Window = glfwCreateWindow(m_Width, m_Height, "Ray Tracing", NULL, NULL);
        if (!m_Window)
        {
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(0);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            return false;

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

        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init("#version 430 core");

        io.Fonts->Build();

		return true;
	}

	void Window::Destroy()
	{
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_Window);
        glfwTerminate();
	}

	bool Window::Update()
	{
        glfwPollEvents();
        glfwSwapBuffers(m_Window);

		return !glfwWindowShouldClose(m_Window);
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
        glfwGetCursorPos(m_Window, &x, &y);
        return { (float)x, (float)y };
    }

    bool Window::IsKeyPressed(int32_t key) const
    {
        return glfwGetKey(m_Window, key) == GLFW_PRESS;
    }

    bool Window::IsMousePressed(int32_t key) const
    {
        return glfwGetMouseButton(m_Window, key) == GLFW_PRESS;
    }

    glm::ivec2 Window::GetSize() const
    {
        int32_t width, height;
        glfwGetWindowSize(m_Window, &width, &height);
        return { width, height };
    }

    void Window::CursorMode(int32_t state) const
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, state);
    }

    void Window::WindowResize()
    {
        glm::ivec2 winSize = GetSize();
        if (winSize.x != m_Width || winSize.y != m_Height)
        {
            m_Width = winSize.x;
            m_Height = winSize.y;
            glViewport(0, 0, m_Width, m_Height);
        }
    }

}
