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

		static Application& Get() { return *s_MainApp; }

	private:
		bool m_ShouldRun;
		float m_LastFrameDuration;
		ImVec2 m_ViewportSize;
		Instance::Window m_MainWindow;
		Render::Renderer m_Renderer;
		Render::Scene m_Scene;

		static Application* s_MainApp;
	};

	Application* CreateApp();

}
