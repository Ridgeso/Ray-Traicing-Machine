#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <imgui.h>
#include <Engine/Core/Base.h>

#include "Engine/Window/Window.h"
#include "Engine/Render/Renderer.h"
#include "Engine/Render/Shader.h"

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

		void run();
		void layout();
		void update();
		void updateView(float ts);

		static Application& Get() { return *MainApp; }

	private:
		ApplicationSpecs specs;
		float lastFrameDuration;
		float appFrameDuration;
		ImVec2 viewportSize;
		Local<Window> mainWindow;
		Local<Renderer> renderer;
		Local<Shader> rtShader;
		Camera camera;
		Scene scene;

		glm::vec2 lastMousePos;
		static Application* MainApp;
	};

	Application* CreateApplication(ApplicationCommandLineArgs args);

}
