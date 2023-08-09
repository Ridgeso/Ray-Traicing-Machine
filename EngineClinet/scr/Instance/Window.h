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

		void LockCursor(bool state) const;

		GLFWwindow* GetNativeWindow() { return m_Window; }

	private:
		int32_t m_Width = 0, m_Height = 0;
		GLFWwindow* m_Window = nullptr;
	};

}
