#include "Application.h"

int main()
{
	RT::Application* app = RT::CreateApp();
	
	app->Run();
	
	delete app;

    return 0;
}
