#pragma once
#include <cstdint>
#include <string>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace RT
{

	struct WindowSpecs
	{
		std::string titel;
		int32_t width, height;
		bool isMinimized;
	};

	class Window
	{
	public:
		Window() = default;
		~Window() = default;

		void Init(const WindowSpecs& specs);
		void ShutDown();

		bool Update();
		bool PullEvents();

		void BeginUI();
		void EndUI();

		glm::vec2 GetMousePos() const;
		bool IsKeyPressed(int32_t key) const;
		bool IsMousePressed(int32_t key) const;
		glm::ivec2 GetSize() const;

		void CursorMode(int32_t state) const;

		GLFWwindow* GetNativeWindow() { return window; }

	private:
		void WindowResize();

	private:
		std::string title;
		int32_t width, height;

		bool isMinimized;

		GLFWwindow* window;
	};

}
