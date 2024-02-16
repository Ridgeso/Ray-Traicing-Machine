#pragma once
#include <cstdint>
#include <string>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "GlfwWindow.fwd.h"
#include "Engine/Window/Window.h"

namespace RT
{

	class GlfwWindow : public Window
	{
	public:
		GlfwWindow() = default;
		~GlfwWindow() = default;
		
		void init(const WindowSpecs& specs) override;
		void shutDown() override;

		bool update() override;
		bool pullEvents() override;

		void beginUI() override;
		void endUI() override;

		glm::vec2 getMousePos() const override;
		bool isKeyPressed(int32_t key) const override;
		bool isMousePressed(int32_t key) const override;
		glm::ivec2 getSize() const override;

		void cursorMode(int32_t state) const override;

	private:
		void initImGui();

	private:
		std::string title;
		int32_t width, height;

		bool isMinimized;

		GLFWwindow* window;
	};

}
