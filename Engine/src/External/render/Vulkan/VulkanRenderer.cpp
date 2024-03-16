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

		device = makeLocal<Device>();
		swapchain = makeLocal<Swapchain>(*device.get(), extent);
		pipeline = std::make_unique<Pipeline>(*device.get());

		device->init(window);
		swapchain->init();
		
		auto pipelineLayoutInfo = VkPipelineLayoutCreateInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		RT_CORE_ASSERT(
			vkCreatePipelineLayout(device->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) == VK_SUCCESS,
			"Could not create pipeline Layout");
		
		auto pipelineConfig = PipelineConfigInfo{};
		auto windowExtent = glm::uvec2(swapchain->getWindowExtent().width, swapchain->getWindowExtent().height);
		Pipeline::defaultPipelineConfigInfo(pipelineConfig, windowExtent);
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
		allocInfo.commandPool = device->getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		RT_CORE_ASSERT(vkAllocateCommandBuffers(device->getDevice(), &allocInfo, commandBuffers.data()) == VK_SUCCESS, "failed to allocate command buffers!");

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

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getGraphicsPipeline());
			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);
			RT_CORE_ASSERT(vkEndCommandBuffer(commandBuffers[i]) == VK_SUCCESS, "failed to record command buffer");
		}
	}

	void VulkanRenderer::shutDown()
	{
		vkDeviceWaitIdle(device->getDevice());

		vkDestroyPipelineLayout(device->getDevice(), pipelineLayout, nullptr);

		vkFreeCommandBuffers(
			device->getDevice(),
			device->getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();

		pipeline->shutdown();
		swapchain->shutdown();
		device->shutdown();
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
