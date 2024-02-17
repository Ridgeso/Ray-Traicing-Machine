#include "Renderer.h"
#include "External/Render/OpenGl/OpenGlRenderer.h"

namespace RT
{

	Local<Renderer> createRenderer()
	{
		return makeLocal<OpenGl::OpenGlRenderer>();
	}

}
