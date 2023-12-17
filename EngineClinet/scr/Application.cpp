#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GLFW/glfw3.h>

#include "Application.h"
#include "Time.h"
#include "Renderer.h"
#include <iostream>

namespace RT
{

	uint32_t pcg_hash(uint32_t input)
	{
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	float FastRandom(uint32_t& seed)
	{
		seed = pcg_hash(seed);
		return (float)seed / std::numeric_limits<uint32_t>::max();
	}

	Application* Application::s_MainApp = nullptr;

	Application::Application()
		: m_ShouldRun(true), m_LastFrameDuration(0.f), m_AppFrameDuration(0), m_ViewportSize(), m_Camera(45.0f, 0.01f, 100.0f)
	{
		s_MainApp = this;
		uint32_t seed = 9423262352u;

        m_ShouldRun &= m_MainWindow.Init();
		glm::ivec2 windowSize = m_MainWindow.GetSize();
		m_ShouldRun &= m_Renderer.Invalidate(windowSize.x, windowSize.y);

		m_Scene.Materials.emplace_back(Render::Material{ { 0.0f, 0.0f, 0.0f }, 0.0, { 0.0f, 0.0f, 0.0f }, 0.0f,  0.0f, 0.0f, 1.0f });
		m_Scene.Materials.emplace_back(Render::Material{ { 1.0f, 1.0f, 1.0f }, 0.0, { 1.0f, 1.0f, 1.0f }, 0.7f,  0.8f, 0.0f, 1.5f });
		m_Scene.Materials.emplace_back(Render::Material{ { 0.2f, 0.5f, 0.7f }, 0.0, { 0.2f, 0.5f, 0.7f }, 0.05f, 0.3f, 0.0f, 1.0f });
		m_Scene.Materials.emplace_back(Render::Material{ { 0.8f, 0.6f, 0.5f }, 0.0, { 0.8f, 0.6f, 0.5f }, 0.0f,  0.3f, 1.0f, 1.0f });
		m_Scene.Materials.emplace_back(Render::Material{ { 0.4f, 0.3f, 0.8f }, 0.0, { 0.8f, 0.6f, 0.5f }, 0.0f,  0.3f, 0.0f, 1.0f });
		
		m_Scene.Spheres.emplace_back(Render::Sphere{ { 0.0f, 0.0f, -2.0f }, 1.0f, 1 });
		m_Scene.Spheres.emplace_back(Render::Sphere{ { 0.0f, -2001.0f, -2.0f }, 2000.0f, 2 });
		m_Scene.Spheres.emplace_back(Render::Sphere{ { 2.5f, 0.0f, -2.0f }, 1.0f, 3 });
		m_Scene.Spheres.emplace_back(Render::Sphere{ { -2.5f, 0.0f, -2.0f }, 1.0f, 4 });

		auto getRandPos = [&seed](float rad) { return FastRandom(seed)* rad - rad / 2; };

		for (int i = 0; i < 70; i++)
		{
			m_Scene.Materials.emplace_back(Render::Material{ });
			m_Scene.Materials[m_Scene.Materials.size() - 1].Albedo = { FastRandom(seed), FastRandom(seed), FastRandom(seed) };
			m_Scene.Materials[m_Scene.Materials.size() - 1].EmmisionColor = { FastRandom(seed), FastRandom(seed), FastRandom(seed) };
			m_Scene.Materials[m_Scene.Materials.size() - 1].Roughness = FastRandom(seed) > 0.9 ? 0.f : FastRandom(seed);
			m_Scene.Materials[m_Scene.Materials.size() - 1].EmmisionPower = FastRandom(seed) > 0.9 ? FastRandom(seed) : 0.f;
			m_Scene.Materials[m_Scene.Materials.size() - 1].RefractionRatio = 1.0f;
		
			m_Scene.Spheres.emplace_back(Render::Sphere{ });
			m_Scene.Spheres[m_Scene.Spheres.size() - 1].Position = { getRandPos(10.0f), -0.75, getRandPos(10.0f) - 2 };
			m_Scene.Spheres[m_Scene.Spheres.size() - 1].Radius = 0.25;
			m_Scene.Spheres[m_Scene.Spheres.size() - 1].MaterialId = m_Scene.Materials.size() - 1;
		}
	
		m_LastMousePos = windowSize / 2;
	}

	Application::~Application()
	{
		m_MainWindow.Destroy();
		m_Renderer.Devalidate();
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
		ImGui::Text("Frames: %d", m_Renderer.GetFrames());
		ImGui::DragInt("Bounces Limit", (int32_t*)&m_Renderer.MaxBounces, 1, 1, 100);
		ImGui::DragInt("Precalculated Frames Limit", (int32_t*)&m_Renderer.MaxFrames, 1, 1, 15);
		if (ImGui::Button("Reset"))
			m_Renderer.ResetFrame();
		ImGui::Checkbox("Accumulate", &m_Renderer.Accumulate);
		ImGui::Checkbox("Draw Environment", &m_Renderer.DrawEnvironment);
		if (ImGui::Button("Add Material"))
			m_Scene.Materials.emplace_back(Render::Material{ { 0.0f, 0.0f, 0.0f }, 0.0, { 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f, 0.0f });
		if (ImGui::Button("Add Sphere"))
			m_Scene.Spheres.emplace_back(Render::Sphere{ { 0.0f, 0.0f, -2.0f }, 1.0f, 0 });
		ImGui::End();

		ImGui::Begin("Scene");
		
		ImGui::Text("Materials:");
		for (size_t i = 1; i < m_Scene.Materials.size(); i++)
		{
			ImGui::PushID((int32_t)i);
			Render::Material& material = m_Scene.Materials[i];

			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			ImGui::ColorEdit3("Emmision Color", glm::value_ptr(material.EmmisionColor));
			ImGui::DragFloat("Roughness", &material.Roughness, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Metalic", &material.Metalic, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Emmision Power", &material.EmmisionPower, 0.005f, 0.0f, std::numeric_limits<float>::max());
			ImGui::DragFloat("Refraction Index", &material.RefractionRatio, 0.005f, 1.0f, 32.0f);

			ImGui::Separator();
			ImGui::PopID();
		}
		
		ImGui::Separator();

		ImGui::Text("Spheres:");
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID((int32_t)i);
			Render::Sphere& sphere = m_Scene.Spheres[i];

			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.01f, 0.0f, std::numeric_limits<float>::max());
			ImGui::SliderInt("Material", &sphere.MaterialId, 1, m_Scene.Materials.size() - 1);

			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		ImVec2 viewPort = ImGui::GetContentRegionAvail();
		if (viewPort.x != m_ViewportSize.x || viewPort.y != m_ViewportSize.y)
		{
			m_ViewportSize = viewPort;
			m_Renderer.ResetFrame();
		}

		ImGui::Image(
			(ImTextureID)m_Renderer.GetDescriptor(),
			m_ViewportSize,
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::End();
		ImGui::PopStyleVar();

		//static bool demo = true;
		//ImGui::ShowDemoWindow(&demo);
	}

	void Application::Render()
	{
		UpdateView(m_AppFrameDuration / 1000.0f);
		glm::ivec2 winSize = m_MainWindow.GetSize();
		m_Renderer.RecreateRenderer(winSize.x, winSize.y);

		Timer timeit;
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
			m_MainWindow.CursorMode(GLFW_CURSOR_DISABLED);
			if (mouseDelta != glm::vec2(0.0f))
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
		{
			m_MainWindow.CursorMode(GLFW_CURSOR_NORMAL);
		}

		if (moved)
		{
			m_Camera.RecalculateInvView();
			m_Renderer.ResetFrame();
		}
	}

	Application* CreateApp()
	{
		return new Application();
	}

}
