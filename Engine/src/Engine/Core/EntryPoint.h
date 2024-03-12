#pragma once

#include "Application.h"
#include <Engine/Core/Log.h>

#define RT_EXIT_STATUS_GOOD 0

#ifdef RAY_TRACING_CLIENT

int main(int argc, char** argv)
{
	RT::Core::Log::init();

	RT::ApplicationCommandLineArgs args = { argc, argv };
	RT::Application* rayTracingApp = RT::CreateApplication(args);
	rayTracingApp->run();
	delete rayTracingApp;

	RT::Core::Log::shutdown();
	return RT_EXIT_STATUS_GOOD;
}

#endif // RAY_TRACEING_CLIENT
