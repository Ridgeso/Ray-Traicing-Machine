#pragma once
#include <cstdint>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace RT::Instance
{

	class Window
	{
	public:
		Window() = default;
		~Window() = default;

		bool Init();
		void Destroy();

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
		int32_t width = 0, height = 0;
		GLFWwindow* window = nullptr;
	};

}
