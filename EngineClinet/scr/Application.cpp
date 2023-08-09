#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GLFW/glfw3.h>

#include "Application.h"
#include "Time.h"
#include "Renderer.h"

namespace RT
{
	Application* Application::s_MainApp = nullptr;

	Application::Application()
		: m_ShouldRun(true), m_LastFrameDuration(0.f), m_AppFrameDuration(0), m_ViewportSize(), m_Camera(45.0f, 0.01f, 100.0f)
	{
		s_MainApp = this;

        m_ShouldRun &= m_MainWindow.Init();

		m_Scene.Spheres.emplace_back(Render::Sphere{ { 0.0f, 0.0f, -2.0f }, 1.0f, { 1.0f, 0.2f, 1.0f, 1.0f } });
		m_Scene.Spheres.emplace_back(Render::Sphere{ { 2.0f, 0.0f, -2.0f }, 1.0f, { 0.2f, 0.5f, 0.7f, 1.0f } });
	
		m_LastMousePos = glm::vec2(0);
	}

	Application::~Application()
	{
		m_MainWindow.Destroy();
	}

	void Application::Run()
	{
		while (m_ShouldRun)
		{
			Timer appTimer;
			Render();
			m_MainWindow.BeginUI();
                Layout();
			m_MainWindow.EndUI();

			m_ShouldRun &= m_MainWindow.Update();
			m_ShouldRun &= m_MainWindow.PullEvents();
			m_AppFrameDuration = appTimer.Ellapsed();
		}
	}

    void Application::Layout()
    {
		ImGui::Begin("Settings");
		ImGui::Text("App frame took: %.3fms", m_AppFrameDuration);
		ImGui::Text("Last render took: %.3fms", m_LastFrameDuration);
		if (ImGui::Button("Render"))
		{ }
		ImGui::End();

		ImGui::Begin("Scene");
		
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID((int32_t)i);
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

	void Application::Render()
	{
		UpdateView(m_LastFrameDuration / 1000.0f);
		Timer timeit;

		m_Renderer.OnResize((int32_t)m_ViewportSize.x, (int32_t)m_ViewportSize.y);
		m_Camera.ResizeCamera((int32_t)m_ViewportSize.x, (int32_t)m_ViewportSize.y);
		m_Renderer.Render(m_Camera, m_Scene);

		m_LastFrameDuration = timeit.Ellapsed();
	}

	void Application::UpdateView(float ts)
	{
		const float speed = 5.0f;
		const float mouseSenisity = 0.003f;
		const float rotationSpeed = 0.3f;
		const glm::vec3 up = glm::vec3(0, 1, 0);
		const glm::vec3& forward = m_Camera.GetDirection();
		glm::vec3 right = glm::cross(forward, up);
		bool moved = false;

		glm::vec2 newMousePos = m_MainWindow.GetMousePos();
		glm::vec2 mouseDelta = (newMousePos - m_LastMousePos) * mouseSenisity;
		m_LastMousePos = newMousePos;

		if (m_MainWindow.IsKeyPressed(GLFW_KEY_W))
		{
			glm::vec3 step = m_Camera.GetPosition() + forward * speed * ts;
			m_Camera.SetPosition(step);
			moved = true;
		}
		if (m_MainWindow.IsKeyPressed(GLFW_KEY_S))
		{
			glm::vec3 step = m_Camera.GetPosition() - forward * speed * ts;
			m_Camera.SetPosition(step);
			moved = true;
		}

		if (m_MainWindow.IsKeyPressed(GLFW_KEY_D))
		{
			glm::vec3 step = m_Camera.GetPosition() + right * speed * ts;
			m_Camera.SetPosition(step);
			moved = true;
		}
		if (m_MainWindow.IsKeyPressed(GLFW_KEY_A))
		{
			glm::vec3 step = m_Camera.GetPosition() - right * speed * ts;
			m_Camera.SetPosition(step);
			moved = true;
		}

		if (m_MainWindow.IsKeyPressed(GLFW_KEY_Q))
		{
			glm::vec3 step = m_Camera.GetPosition() + up * speed * ts;
			m_Camera.SetPosition(step);
			moved = true;
		}
		if (m_MainWindow.IsKeyPressed(GLFW_KEY_E))
		{
			glm::vec3 step = m_Camera.GetPosition() - up * speed * ts;
			m_Camera.SetPosition(step);
			moved = true;
		}

		
		if (m_MainWindow.IsMousePressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			m_MainWindow.LockCursor(true);
			if (mouseDelta.x != 0.f || mouseDelta.y != 0.f)
			{
				mouseDelta *= rotationSpeed;
				glm::quat q = glm::normalize(glm::cross(
					glm::angleAxis(-mouseDelta.y, right),
					glm::angleAxis(-mouseDelta.x, up)
				));
				m_Camera.SetDirection(glm::rotate(q, m_Camera.GetDirection()));
				moved = true;
			}
		}
		else
			m_MainWindow.LockCursor(false);

		if (moved)
		{
			m_Camera.RecalculateInvView();
			m_Camera.RecalculateCashedCoords();
		}
	}

	Application* CreateApp()
	{
		return new Application();
	}

}
