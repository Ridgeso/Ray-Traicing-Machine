#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <imgui.h>

#include "Window.h"
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

		static Application& Get() { return *MainApp; }

	private:
		bool shouldRun;
		float lastFrameDuration;
		float appFrameDuration;
		ImVec2 viewportSize;
		Window mainWindow;
		Render::Renderer renderer;
		Render::Camera camera;
		Render::Scene scene;

		glm::vec2 lastMousePos;
		static Application* MainApp;
	};

	Application* CreateApp();

}
