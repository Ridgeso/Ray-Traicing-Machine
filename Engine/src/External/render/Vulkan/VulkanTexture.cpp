#include "VulkanTexture.h"
#include "Engine/Core/Assert.h"

#include "Device.h"
#include "Swapchain.h"
#include "utils/Utils.h"

#include <backends/imgui_impl_vulkan.h>

namespace RT::Vulkan
{

	VulkanTexture::VulkanTexture(const glm::ivec2 size, const ImageFormat imageFormat)
	{
	}

	VulkanTexture::~VulkanTexture()
	{
	}

	void VulkanTexture::setBuff(const void* data)
	{
	}

	void VulkanTexture::bind(const uint32_t slot) const
	{
	}

	const uint32_t VulkanTexture::getTexId() const
	{
		return 0;
	}

	const int32_t VulkanTexture::getWidth() const
	{
		return 0;
	}

	const int32_t VulkanTexture::getHeight() const
	{
		return 0;
	}

	auto vulkanFormat = VK_FORMAT_R8G8B8A8_UNORM;

	void VulkanTexture::init(uint32_t width, uint32_t height, const ImageFormat imageFormat)
	{
		this->width = width;
		this->height = height;
		this->format = imageFormat;

		createImage();
		createImageView();
		createSampler();
		allocateMemory();

		// Create the Descriptor Set: // WAZNE !!!
		descriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(
			sampler,
			imageView,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void VulkanTexture::shutdown()
	{
		auto device = DeviceInstance.getDevice();

		vkDestroySampler(device, sampler, nullptr);
		vkDestroyImageView(device, imageView, nullptr);
		vkDestroyImage(device, image, nullptr);
		vkFreeMemory(device, memory, nullptr);
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void VulkanTexture::createImage()
	{
		auto imageCreateInfo = VkImageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.extent.width = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.flags = 0;

		if (ImageFormat::Stencil == format)
		{
			imageCreateInfo.format = Swapchain::findDepthFormat();
			imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		else
		{
			imageCreateInfo.format = vulkanFormat;
			imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		RT_CORE_ASSERT(
			vkCreateImage(DeviceInstance.getDevice(), &imageCreateInfo, nullptr, &image) == VK_SUCCESS,
			"failed to create texture image!");
	}

	void VulkanTexture::createImageView()
	{
		auto viewInfo = VkImageViewCreateInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		if (ImageFormat::Stencil == format)
		{
			viewInfo.format = Swapchain::findDepthFormat();
		}
		else
		{
			viewInfo.format = vulkanFormat;
		}
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		RT_CORE_ASSERT(
			vkCreateImageView(DeviceInstance.getDevice(), &viewInfo, nullptr, &imageView) == VK_SUCCESS,
			"failed to create texture image view!");
	}

	void VulkanTexture::createSampler()
	{
		auto info = VkSamplerCreateInfo{};
		info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		info.magFilter = VK_FILTER_LINEAR;
		info.minFilter = VK_FILTER_LINEAR;
		info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		info.minLod = -1000;
		info.maxLod = 1000;
		info.maxAnisotropy = 1.0f;
		RT_CORE_ASSERT(
			vkCreateSampler(DeviceInstance.getDevice(), &info, nullptr, &sampler) == VK_SUCCESS,
			"failed to create texture image sampler!");
	}

	void VulkanTexture::allocateMemory()
	{
		auto device = DeviceInstance.getDevice();

		auto memReq = VkMemoryRequirements{};
		vkGetImageMemoryRequirements(device, image, &memReq);

		auto allocInfo = VkMemoryAllocateInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memReq.size;
		allocInfo.memoryTypeIndex = DeviceInstance.findMemoryType(
			memReq.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		RT_CORE_ASSERT(
			vkAllocateMemory(device, &allocInfo, nullptr, &memory) == VK_SUCCESS,
			"failed to allocate image memory!");
		RT_CORE_ASSERT(
			vkBindImageMemory(device, image, memory, 0) == VK_SUCCESS,
			"failed to bind image memory!");
	}

}
