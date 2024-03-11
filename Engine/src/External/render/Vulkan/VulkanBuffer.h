#pragma once
#include "Engine/Render/Buffer.h"

namespace RT::Vulkan
{

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(const uint32_t size);
		VulkanVertexBuffer(const uint32_t size, const void* data);
		~VulkanVertexBuffer() final;

		void registerAttributes(const VertexElements& elements) const final;
		void setData(const uint32_t size, const void* data) const final;
		const int32_t getCount() const final { return 0; }

		void bind() const final;
		void unbind() const final;

	private:
	};

}
