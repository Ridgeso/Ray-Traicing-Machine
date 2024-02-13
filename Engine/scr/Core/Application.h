#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <imgui.h>

#include "Window.h"
#include "Renderer.h"

namespace RT
{

	struct ApplicationCommandLineArgs
	{
		int32_t argc;
		char** argv;
	};

	struct ApplicationSpecs
	{
		std::string name;
		bool isRunning;
		ApplicationCommandLineArgs args;
	};

	class Application
	{
	public:
		Application(ApplicationSpecs specs);
		virtual ~Application();

		void Run();
		void Layout();
		void Render();
		void UpdateView(float ts);

		static Application& Get() { return *MainApp; }

	private:
		ApplicationSpecs specs;
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

	Application* CreateApplication(ApplicationCommandLineArgs args);

}
