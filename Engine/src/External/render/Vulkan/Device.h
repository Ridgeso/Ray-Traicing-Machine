#pragma once
#include <array>
#include <vector>

#include <vulkan/vulkan.h>

#include "Engine/Window/Window.h"
#include "utils/Utils.h"

namespace RT::Vulkan
{

    class Device
    {
    public:
        Device() = default;
        ~Device() = default;

        void init(Window& window);
        void shutdown();

        void createImageWithInfo(
            const VkImageCreateInfo& imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage& image,
            VkDeviceMemory& imageMemory) const;
        VkFormat findSupportedFormat(
            const std::vector<VkFormat>& candidates,
            VkImageTiling tiling,
            VkFormatFeatureFlags features) const;

        VkDevice getDevice() const { return device; }
        VkSurfaceKHR getSurface() const { return surface; }
        VkQueue getGraphicsQueue() const { return graphicsQueue; }
        VkQueue getPresentQueue() const { return presentQueue; }
        VkCommandPool getCommandPool() const { return commandPool; }
        
        const Utils::SwapChainSupportDetails& getSwapChainSupportDetails() const
        { return swapChainSupportDetails; }
        Utils::QueueFamilyIndices getQueueFamilyIndices() const { return queueFamilyIndices; }

    private:
        void createInstance();
        void createSurface(Window& window);
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();

        void validateRequiredInstanceExtensions() const;
        bool isDeviceSuitable(VkPhysicalDevice phyDev);
        Utils::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice phyDev) const;
        Utils::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice phyDev);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

        static bool checkDeviceExtensionSupport(VkPhysicalDevice phyDev);

    private:
        VkDevice device = {};
        VkInstance instance = {};
        VkSurfaceKHR surface = {};
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        VkQueue graphicsQueue = {};
        VkQueue presentQueue = {};
        VkCommandPool commandPool = {};
        VkPhysicalDeviceProperties properties = {};

        Utils::SwapChainSupportDetails swapChainSupportDetails = {};
        Utils::QueueFamilyIndices queueFamilyIndices = {};

        static constexpr std::array<const char*, 1> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    };

}
