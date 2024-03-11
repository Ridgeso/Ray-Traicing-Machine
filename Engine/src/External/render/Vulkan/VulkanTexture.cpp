#include "VulkanTexture.h"

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

}
