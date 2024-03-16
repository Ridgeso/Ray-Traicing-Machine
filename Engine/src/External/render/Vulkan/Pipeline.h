#pragma once
#include <vulkan/vulkan.h>

#include "Device.h"

namespace RT::Vulkan
{

    struct PipelineConfigInfo
    {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        VkViewport viewport = {};
        VkRect2D scissors = {};
        VkPipelineViewportStateCreateInfo viewportInfo = {};
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
        VkPipelineRasterizationStateCreateInfo rasterizationInfo = {};
        VkPipelineMultisampleStateCreateInfo multisampleInfo = {};
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

	class Pipeline
	{
    public:
        Pipeline(Device& device);
        ~Pipeline();

        void init(
            const std::string& vertFilepath,
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);
        void shutdown();
        
        VkPipeline getGraphicsPipeline() const { return graphicsPipeline; }
        
        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo, const glm::uvec2 size);

    private:
        void createGraphicsPipeline(
            const std::string& vertFilepath,
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);
        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) const;
        static std::vector<char> readFile(const std::string& filepath);

    private:
        Device& device;
        VkPipeline graphicsPipeline = {};
        VkShaderModule vertShaderModule = {};
        VkShaderModule fragShaderModule = {};
	};

}
