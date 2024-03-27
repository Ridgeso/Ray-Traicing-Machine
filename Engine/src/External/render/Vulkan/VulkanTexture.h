#pragma once
#include "Engine/Render/Texture.h"

#include <vulkan/vulkan.h>

namespace RT::Vulkan
{

	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const glm::ivec2 size, const ImageFormat imageFormat);
		~VulkanTexture() final;

		void setBuff(const void* data)  final;

		void bind(const uint32_t slot = 0) const final;

		const uint32_t getTexId() const final;
		const int32_t getWidth() const final;
		const int32_t getHeight() const final;

		void init(uint32_t width, uint32_t height, const ImageFormat imageFormat);
		void shutdown();

		VkImageView getImageView() const { return imageView; }

	private:
		void createImage();
		void createImageView();
		void createSampler();
		void allocateMemory();
	
	private:
		uint32_t width = 0, height = 0;

		VkImage image = {};
		VkImageView imageView = {};
		VkDeviceMemory memory = {};
		VkSampler sampler = {};

		ImageFormat format = ImageFormat::RGBA8;

		// TODO: will be needed when writing to image memory
		VkBuffer stagingBuffer = {};
		VkDeviceMemory stagingBufferMemory = {};

		size_t alignedSize = 0;

		VkDescriptorSet descriptorSet = {};
	};

}
