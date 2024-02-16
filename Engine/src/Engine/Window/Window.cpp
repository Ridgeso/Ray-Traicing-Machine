#include "Window.h"
#include "External/window/GlfwWindow/GlfwWindow.h"

namespace RT
{

	Local<Window> createWindow()
	{
		return makeLocal<GlfwWindow>();
	}

}
