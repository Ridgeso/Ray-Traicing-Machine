#pragma once
#include "Engine/Render/FrameBuffer.h"
#include "VulkanTexture.h"

namespace RT::Vulkan
{
	class VulkanFrameBuffer : public FrameBuffer
	{
	public:
		VulkanFrameBuffer(const glm::ivec2 size, const int32_t numOfAttachments);
		~VulkanFrameBuffer() final;

		void bind() const final;
		void unbind() const final;

		const Texture& getAttachment(const uint32_t index = 0) const final;

	private:
		VulkanTexture fakeAttachment;
	};
}
