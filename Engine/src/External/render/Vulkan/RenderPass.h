#pragma once
#include <vector>

#include "VulkanFrameBuffer.h"

#include <vulkan/vulkan.h>

namespace RT::Vulkan
{

	class Swapchain;

	class RenderPass
	{
	public:
		RenderPass() = default;
		~RenderPass() = default;

		void init(
			Swapchain& swapchin,
			std::vector<VkImageView>& framesAttachments,
			VkExtent2D extent,
			uint32_t imageCount);
		void shutdown();

		const VulkanFrameBuffer& getFrameBuffer(const uint32_t idx) const { return frameBuffers[idx]; }

	private:
		VkRenderPass renderPass = {};
		std::vector<VulkanFrameBuffer> frameBuffers = {};
	};

}
