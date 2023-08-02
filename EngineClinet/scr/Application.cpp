#include <glm/gtc/type_ptr.hpp>

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

		m_Scene.Spheres.emplace_back(Render::Sphere{ { 0.0, 0.0, -2.0 }, 1.0f, { 1.0, 0.2, 1.0 } });
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
		Timer timeit;
		m_Renderer.OnResize((int32_t)m_ViewportSize.x, (int32_t)m_ViewportSize.y);
		m_Renderer.Render(m_Scene);
		m_LastFrameDuration = timeit.Ellapsed();

		ImGui::Begin("Settings");
		ImGui::Text("Last render took: %.3fms", m_LastFrameDuration);
		if (ImGui::Button("Render"))
		{
		}

		ImGui::End();

		ImGui::Begin("Scene");
		
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);
			Render::Sphere& sphere = m_Scene.Spheres[i];

			ImGui::DragFloat3("Position", glm::value_ptr(sphere.position), 0.1f);
			ImGui::DragFloat("Radius", &sphere.radius, 0.1f);
			ImGui::ColorEdit3("Color", glm::value_ptr(sphere.color));

			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportSize = ImGui::GetContentRegionAvail();
		auto& image = m_Renderer.GetRenderedImage();
		ImGui::Image(
			(ImTextureID)image.GetTexId(),
			{ (float)image.GetWidth(), (float)image.GetHeight() },
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::End();
		ImGui::PopStyleVar();
	}

	Application* CreateApp()
	{
		return new Application();
	}

}
