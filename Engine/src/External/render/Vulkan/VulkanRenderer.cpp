#include "External/render/Vulkan/VulkanRenderer.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Application.h"

namespace RT::Vulkan
{

	void VulkanRenderer::init(const RenderSpecs& specs)
	{
		auto& window = Application::Get().getWindow();
		auto size = window.get()->getSize();
		extent = VkExtent2D{ (uint32_t)size.x, (uint32_t)size.y };

		device = makeLocal<Device>(window.get());
		swapchain = makeLocal<Swapchain>(device.get(), extent);
		
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

			RT_CORE_ASSERT(vkCreatePipelineLayout(device->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) == VK_SUCCESS, "Could not create pipeline Layout");
		
		PipelineConfigInfo pipelineConfig{};
		Pipeline::defaultPipelineConfigInfo(pipelineConfig, swapchain->windowExtent.width, swapchain->windowExtent.height);
		pipelineConfig.renderPass = swapchain->renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		pipeline = std::make_unique<Pipeline>(
			device.get(),
			"..\\Engine\\assets\\shaders\\vert.vert.spv",
			"..\\Engine\\assets\\shaders\\frag.frag.spv",
			pipelineConfig);

		commandBuffers.resize(swapchain->swapChainImages.size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = device->commandPool;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		RT_CORE_ASSERT(vkAllocateCommandBuffers(device->device, &allocInfo, commandBuffers.data()) == VK_SUCCESS, "failed to allocate command buffers!");

		for (int32_t i = 0; i < commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			RT_CORE_ASSERT(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) == VK_SUCCESS, "failed to begin command buffer!");




			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = swapchain->renderPass;
			renderPassInfo.framebuffer = swapchain->swapChainFramebuffers[i];

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapchain->windowExtent;

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = clearValues.size();
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);
			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);
			RT_CORE_ASSERT(vkEndCommandBuffer(commandBuffers[i]) == VK_SUCCESS, "failed to record command buffer");
		}
	}

	void VulkanRenderer::shutDown()
	{
		vkDeviceWaitIdle(device->device);

		vkDestroyPipelineLayout(device->device, pipelineLayout, nullptr);

		vkFreeCommandBuffers(
			device->device,
			device->commandPool,
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	void VulkanRenderer::render(const Camera& camera, const Shader& shader, const VertexBuffer& vbuffer, const Scene& scene)
	{
		uint32_t imageIndex;
		auto result = swapchain->acquireNextImage(&imageIndex);

		//const bool test = result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR;
		RT_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "failte to acquire swap chain image!");

		result = swapchain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		RT_ASSERT(result == VK_SUCCESS, "failte to present swap chain image!");
	}

}
