#define ENTRY_POINT
#include <Engine.h>

namespace RT
{
	
	class RayTracingClient : public Application
	{
	public:
		RayTracingClient(ApplicationSpecs specs)
			: Application(specs)
		{}

		~RayTracingClient() {}

	private:

	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecs specs = { "Ray Tracing", args };

		return new RayTracingClient(specs);
	}

}
