#include "RenderBase.h"
#include "FrameBuffer.h"

#include "External/Render/OpenGl/OpenGlFrameBuffer.h"
#include "External/Render/Vulkan/VulkanFrameBuffer.h"

namespace RT
{

	Share<FrameBuffer> FrameBuffer::create(const glm::ivec2 size, const int32_t numOfAttachments)
	{
		switch (GlobalRenderAPI)
		{
			case RenderAPI::OpenGL: return makeShare<OpenGl::OpenGlFrameBuffer>(size, numOfAttachments);
			case RenderAPI::Vulkan: return makeShare<Vulkan::VulkanFrameBuffer>(size, numOfAttachments);
		}
		return nullptr;
	}

	FrameBuffer::~FrameBuffer() { }

}
