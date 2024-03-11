#include "RenderBase.h"
#include "Buffer.h"

#include "External/Render/OpenGl/OpenGlBuffer.h"
#include "External/Render/Vulkan/VulkanBuffer.h"

namespace RT
{

	Local<VertexBuffer> VertexBuffer::create(const uint32_t size)
	{
		switch (GlobalRenderAPI)
		{
			case RenderAPI::OpenGL: return makeLocal<OpenGl::OpenGlVertexBuffer>(size);
			case RenderAPI::Vulkan: return makeLocal<Vulkan::VulkanVertexBuffer>(size);
		}
		return nullptr;
	}

	Local<VertexBuffer> VertexBuffer::create(const uint32_t size, const void* data)
	{
		switch (GlobalRenderAPI)
		{
			case RenderAPI::OpenGL: return makeLocal<OpenGl::OpenGlVertexBuffer>(size, data);
			case RenderAPI::Vulkan: return makeLocal<Vulkan::VulkanVertexBuffer>(size, data);
		}
		return nullptr;
	}

	VertexBuffer::~VertexBuffer() {}

	namespace Utils
	{
	
		int32_t getNrOfComponents(const VertexElement element)
		{
			switch (element)
			{
				case VertexElement::Int:
				case VertexElement::Float:
					return 1;
				case VertexElement::Int2:
				case VertexElement::Float2:
					return 2;
				case VertexElement::Int3:
				case VertexElement::Float3:
					return 3;
				case VertexElement::Int4:
				case VertexElement::Float4:
					return 4;
			}
			return 0;
		}

	}

}
