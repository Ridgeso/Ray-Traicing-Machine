#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Render/Renderer.h"

#include <vulkan/vulkan.h>
#include "Device.h"
#include "Swapchain.h"
#include "Pipeline.h"

namespace RT::Vulkan
{
	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer() = default;
		~VulkanRenderer() = default;

		void init(const RenderSpecs& specs) final;
		void shutDown() final;

		void render(const Camera& camera, const Shader& shader, const VertexBuffer& vbuffer, const Scene& scene) final;
	private:
		Local<Device> device;
		Local<Swapchain> swapchain;

		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		Local<Pipeline> pipeline;
		
		VkExtent2D extent;
	};
}
