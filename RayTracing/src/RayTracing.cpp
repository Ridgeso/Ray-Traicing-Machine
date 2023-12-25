#define ENTRY_POINT
#include "Engine.h"
#include <iostream>

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
		return new RayTracingClient({});
	}
}
