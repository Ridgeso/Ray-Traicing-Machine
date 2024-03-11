#include "VulkanFrameBuffer.h"

namespace RT::Vulkan
{

	VulkanFrameBuffer::VulkanFrameBuffer(const glm::ivec2 size, const int32_t numOfAttachments)
		: fakeAttachment{{0, 0}, ImageFormat::R8}
	{
	}

	VulkanFrameBuffer::~VulkanFrameBuffer()
	{
	}

	void VulkanFrameBuffer::bind() const
	{
	}

	void VulkanFrameBuffer::unbind() const
	{
	}

	const Texture& VulkanFrameBuffer::getAttachment(const uint32_t index) const
	{
		return fakeAttachment;
	}

}
