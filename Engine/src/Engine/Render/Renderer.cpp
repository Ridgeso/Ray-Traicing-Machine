#include "Renderer.h"
#include "External/render/OpenGl/OpenGlRenderer.h"

namespace RT
{

	Local<Renderer> createRenderer()
	{
		return makeLocal<OpenGlRenderer>();
	}

}
