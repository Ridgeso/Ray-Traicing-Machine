#pragma once
#include <cstdint>

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

		GLFWwindow* GetNativeWindow() { return m_Window; }

	private:
		int32_t m_Width = 0, m_Height = 0;
		GLFWwindow* m_Window = nullptr;
	};

}
