#include "VulkanFrameBuffer.h"
#include "Engine/Core/Assert.h"

#include "Device.h"

namespace RT::Vulkan
{
	
	VulkanFrameBuffer::VulkanFrameBuffer()
		: attachments{1}
	{
	}

	VulkanFrameBuffer::VulkanFrameBuffer(const glm::ivec2 size, const int32_t numOfAttachments)
		: attachments{1}
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

	const Texture& VulkanFrameBuffer::getAttachment(const uint32_t idx) const
	{
		return attachments[idx];
	}

	void VulkanFrameBuffer::init(
		std::vector<VkImageView>& attachmentsView,
		VkRenderPass renderPass,
		VkExtent2D extent)
	{
		auto framebufferInfo = VkFramebufferCreateInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachmentsView.size());
		framebufferInfo.pAttachments = attachmentsView.data();
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		RT_CORE_ASSERT(
			vkCreateFramebuffer(
				DeviceInstance.getDevice(),
				&framebufferInfo,
				nullptr,
				&frameBuffer) == VK_SUCCESS,
			"failed to create framebuffer!");
	}

	void VulkanFrameBuffer::shutdown()
	{
		vkDestroyFramebuffer(DeviceInstance.getDevice(), frameBuffer, nullptr);
		for (auto& attachment : attachments)
		{
			attachment.shutdown();
		}
		attachments.clear();
	}

}
