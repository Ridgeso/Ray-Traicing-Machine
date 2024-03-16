#pragma once
#include <vector>

#include "Engine/Render/Buffer.h"

#include "Device.h"

#define GLM_FORCE_RADAINS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace RT::Vulkan
{

	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 color;

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
	};

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

		void bind(const VkCommandBuffer commandBuffer) const;
		void draw(const VkCommandBuffer commandBuffer) const;

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);

	private:
		VkBuffer vertexBuffer = {};
		VkDeviceMemory vertexMemory = {};
		uint32_t vertexCount = 0u;
	};

}
