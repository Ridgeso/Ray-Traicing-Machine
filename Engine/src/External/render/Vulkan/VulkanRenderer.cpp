#include "External/render/Vulkan/VulkanRenderer.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Application.h"

#include "utils/Debug.h"

namespace RT::Vulkan
{

	VulkanRenderer::VulkanRenderer()
	{
		if (EnableValidationLayers)
		{
			RT_CORE_ASSERT(checkValidationLayerSupport(), "validation layers requested, but not available!");
		}
	}

	void VulkanRenderer::init(const RenderSpecs& specs)
	{
		auto& window = *Application::Get().getWindow();
		auto size = window.getSize();
		extent = VkExtent2D{ (uint32_t)size.x, (uint32_t)size.y };

		auto& deviceInstance = DeviceInstance;

		swapchain = makeLocal<Swapchain>(extent);
		pipeline = std::make_unique<Pipeline>();

		deviceInstance.init(window);
		swapchain->init();
		
		auto triangleVert = std::vector<Vertex>{
			{{0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}},
			{{-0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}}
		};
		vertexBuffer = makeLocal<VulkanVertexBuffer>(
			static_cast<uint32_t>(triangleVert.size()),
			triangleVert.data());

		auto pipelineLayoutInfo = VkPipelineLayoutCreateInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		RT_CORE_ASSERT(
			vkCreatePipelineLayout(deviceInstance.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) == VK_SUCCESS,
			"Could not create pipeline Layout");
		
		auto pipelineConfig = PipelineConfigInfo{};
		Pipeline::defaultPipelineConfigInfo(pipelineConfig, swapchain->getWindowExtent());
		pipelineConfig.renderPass = swapchain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		pipeline->init(
			"..\\Engine\\assets\\shaders\\vert.vert.spv",
			"..\\Engine\\assets\\shaders\\frag.frag.spv",
			pipelineConfig);

		commandBuffers.resize(swapchain->getSwapChainImages().size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = deviceInstance.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		RT_CORE_ASSERT(vkAllocateCommandBuffers(deviceInstance.getDevice(), &allocInfo, commandBuffers.data()) == VK_SUCCESS, "failed to allocate command buffers!");

		for (int32_t i = 0; i < commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			RT_CORE_ASSERT(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) == VK_SUCCESS, "failed to begin command buffer!");

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = swapchain->getRenderPass();
			renderPassInfo.framebuffer = swapchain->getSwapChainFramebuffers()[i];

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapchain->getWindowExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = clearValues.size();
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			pipeline->bind(commandBuffers[i]);
			vertexBuffer->bind(commandBuffers[i]);
			vertexBuffer->draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);
			RT_CORE_ASSERT(vkEndCommandBuffer(commandBuffers[i]) == VK_SUCCESS, "failed to record command buffer");
		}
	}

	void VulkanRenderer::shutDown()
	{
		auto& deviceInstance = DeviceInstance;

		vkDeviceWaitIdle(deviceInstance.getDevice());
		vkDestroyPipelineLayout(deviceInstance.getDevice(), pipelineLayout, nullptr);
		vkFreeCommandBuffers(
			deviceInstance.getDevice(),
			deviceInstance.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();

		pipeline->shutdown();
		
		vertexBuffer.reset();

		swapchain->shutdown();
		deviceInstance.shutdown();
	}

	void VulkanRenderer::render(const Camera& camera, const Shader& shader, const VertexBuffer& vbuffer, const Scene& scene)
	{
		uint32_t imageIndex = 0u;
		auto result = swapchain->acquireNextImage(imageIndex);
		RT_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "failte to acquire swap chain image!");
		result = swapchain->submitCommandBuffers(commandBuffers[imageIndex], imageIndex);
		RT_ASSERT(result == VK_SUCCESS, "failte to present swap chain image!");
	}

}
