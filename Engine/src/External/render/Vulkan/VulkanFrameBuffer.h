#pragma once
#include <vector>

#include "Engine/Render/FrameBuffer.h"
#include "VulkanTexture.h"

#include <vulkan/vulkan.h>

namespace RT::Vulkan
{
	class VulkanFrameBuffer : public FrameBuffer
	{
	public:
		VulkanFrameBuffer();
		VulkanFrameBuffer(const glm::ivec2 size, const int32_t numOfAttachments);
		~VulkanFrameBuffer() final;

		void bind() const final;
		void unbind() const final;

		const Texture& getAttachment(const uint32_t idx = 0) const final;

		void init(
			std::vector<VkImageView>& attachmentsView,
			VkRenderPass renderPass,
			VkExtent2D extent);
		void shutdown();

		VkFramebuffer getFrameBuffer() const { return frameBuffer; }

	private:
		std::vector<VulkanTexture> attachments;

		VkFramebuffer frameBuffer = {};
	};
}
