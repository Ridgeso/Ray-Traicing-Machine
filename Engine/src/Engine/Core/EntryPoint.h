#pragma once

#include "Application.h"

#define RT_EXIT_STATUS_GOOD 0

#ifdef RAY_TRACING_CLIENT

int main(int argc, char** argv)
{
	RT::ApplicationCommandLineArgs args = { argc, argv };
	RT::Application* rayTracingApp = RT::CreateApplication(args);
	rayTracingApp->run();
	delete rayTracingApp;
	return RT_EXIT_STATUS_GOOD;
}

#endif // RAY_TRACEING_CLIENT
