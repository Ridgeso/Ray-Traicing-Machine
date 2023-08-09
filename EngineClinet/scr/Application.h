#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <imgui.h>

#include "Instance/Window.h"
#include "Renderer.h"

namespace RT
{

	class Application
	{
	public:
		Application();
		~Application();

		void Run();
		void Layout();
		void Render();
		void UpdateView(float ts);

		static Application& Get() { return *s_MainApp; }

	private:
		bool m_ShouldRun;
		float m_LastFrameDuration;
		float m_AppFrameDuration;
		ImVec2 m_ViewportSize;
		Instance::Window m_MainWindow;
		Render::Renderer m_Renderer;
		Render::Camera m_Camera;
		Render::Scene m_Scene;

		glm::vec2 m_LastMousePos;
		static Application* s_MainApp;
	};

	Application* CreateApp();

}
