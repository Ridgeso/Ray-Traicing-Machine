#include "RenderBase.h"
#include "Renderer.h"
#include "External/Render/OpenGl/OpenGlRenderer.h"
#include "External/Render/Vulkan/VulkanRenderer.h"

namespace RT
{

	RenderAPI GlobalRenderAPI = RenderAPI::OpenGL;

	Local<Renderer> createRenderer()
	{
		switch (GlobalRenderAPI)
		{
			case RenderAPI::OpenGL: return makeLocal<OpenGl::OpenGlRenderer>();
			case RenderAPI::Vulkan: return makeLocal<Vulkan::VulkanRenderer>();
		}
		return nullptr;
	}

}
