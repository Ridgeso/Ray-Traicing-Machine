#define ENTRY_POINT
#include <Engine/Engine.h>

class RayTracingClient : public RT::Application
{
public:
	RayTracingClient(RT::ApplicationSpecs specs)
		: Application(specs)
	{}

	~RayTracingClient() {}

private:

};

RT::Application* RT::CreateApplication(RT::ApplicationCommandLineArgs args)
{
	RT::ApplicationSpecs specs = { "Ray Tracing", true, args };

	return new RayTracingClient(specs);
}
