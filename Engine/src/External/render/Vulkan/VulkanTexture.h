#pragma once
#include "Engine/Render/Texture.h"

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
	private:
	};

}
