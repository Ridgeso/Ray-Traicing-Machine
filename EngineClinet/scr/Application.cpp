#include "Application.h"
#include "Time.h"
#include "Renderer.h"

namespace RT
{
	Application* Application::s_MainApp = nullptr;

	Application::Application()
		: m_ShouldRun(true), m_LastFrameDuration(0.f), m_ViewportSize()
	{
		s_MainApp = this;

        m_ShouldRun &= m_MainWindow.Init();
	}

	Application::~Application()
	{
		m_MainWindow.Destroy();
	}

	void Application::Run()
	{
		while (m_ShouldRun)
		{
			m_MainWindow.BeginUI();
                Layout();
			m_MainWindow.EndUI();

			m_ShouldRun &= m_MainWindow.Update();
			m_ShouldRun &= m_MainWindow.PullEvents();
		}
	}

    void Application::Layout()
    {
		ImGui::Begin("Settings");
		ImGui::Text("Last render took: %.3fms", m_LastFrameDuration);
		if (ImGui::Button("Render"))
		{
		}

		ImGui::End();

		ImGui::Begin("Scene");

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

			m_ViewportSize = ImGui::GetContentRegionAvail();
			auto image = m_Renderer.GetRenderedImage();
			if (image.GetWidth() != 0 && image.GetHeight() != 0)
				ImGui::Image(
					(ImTextureID)image.GetTexId(),
					{ (float)image.GetWidth(), (float)image.GetHeight() },
					ImVec2(0, 1),
					ImVec2(1, 0)
				);

		ImGui::End();
		ImGui::PopStyleVar();

		Timer timeit;
		m_Renderer.OnResize((int32_t)m_ViewportSize.x, (int32_t)m_ViewportSize.y);
		m_Renderer.Render();
		m_LastFrameDuration = timeit.Ellapsed();
	}

	Application* CreateApp()
	{
		return new Application();
	}

}
