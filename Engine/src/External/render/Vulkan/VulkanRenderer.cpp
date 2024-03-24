#include "External/render/Vulkan/VulkanRenderer.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Application.h"

#include "utils/Debug.h"

#include "GLFW/glfw3.h"

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

		pipeline = std::make_unique<Pipeline>();
		deviceInstance.init(window);
		recreateSwapchain();
		
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
		Pipeline::defaultPipelineConfigInfo(pipelineConfig);
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

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
		}

		RT_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "failte to acquire swap chain image!");
		
		recordCommandbuffer(imageIndex);

		result = swapchain->submitCommandBuffers(commandBuffers[imageIndex], imageIndex);
		
		if (VK_ERROR_OUT_OF_DATE_KHR == result || VK_SUBOPTIMAL_KHR == result)
		{
			recreateSwapchain();
			return;
		}

		RT_ASSERT(result == VK_SUCCESS, "failte to present swap chain image!");
	}

	void VulkanRenderer::recordCommandbuffer(const uint32_t imIdx)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		RT_CORE_ASSERT(vkBeginCommandBuffer(commandBuffers[imIdx], &beginInfo) == VK_SUCCESS, "failed to begin command buffer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapchain->getRenderPass();
		renderPassInfo.framebuffer = swapchain->getSwapChainFramebuffers()[imIdx];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain->getWindowExtent();

		constexpr auto clearValues = std::array<VkClearValue, 2>{
			VkClearValue{ { 0.1f, 0.1f, 0.1f, 1.0f } }, // color
			VkClearValue{ { 1.0f, 0 } } // depthStencil
		};
		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imIdx], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		auto viewport = VkViewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = swapchain->getSwapchainExtent().width;
		viewport.height = swapchain->getSwapchainExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		auto scissor = VkRect2D{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchain->getSwapchainExtent();
		vkCmdSetViewport(commandBuffers[imIdx], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imIdx], 0, 1, &scissor);

		pipeline->bind(commandBuffers[imIdx]);
		vertexBuffer->bind(commandBuffers[imIdx]);
		vertexBuffer->draw(commandBuffers[imIdx]);

		vkCmdEndRenderPass(commandBuffers[imIdx]);
		RT_CORE_ASSERT(vkEndCommandBuffer(commandBuffers[imIdx]) == VK_SUCCESS, "failed to record command buffer");
	}

	void VulkanRenderer::recreateSwapchain()
	{
		auto& window = *Application::Get().getWindow();
		auto size = window.getSize();
		while (size.x == 0 || size.y == 0)
		{
			size = window.getSize();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(DeviceInstance.getDevice());
		extent = VkExtent2D{ (uint32_t)size.x, (uint32_t)size.y };

		auto oldSwapchain = Share<Swapchain>(nullptr);
		if (swapchain == nullptr)
		{
			swapchain = makeLocal<Swapchain>(extent);
		}
		else
		{
			// TODO: check rendepass compatibility
			oldSwapchain = Share<Swapchain>(swapchain.release());
			swapchain = makeLocal<Swapchain>(extent, oldSwapchain);
		}
		
		swapchain->init();

		if (oldSwapchain)
		{
			RT_CORE_ASSERT(swapchain->compareFormats(*oldSwapchain), "swapchain image/depth formats has changed");
			oldSwapchain->shutdown();
		}
	}

}
