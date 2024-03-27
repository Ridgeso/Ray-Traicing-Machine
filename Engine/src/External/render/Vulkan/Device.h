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
        ~Device() = default;
        
        Device(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(const Device&) = delete;
        Device&& operator=(Device&&) = delete;

        static Device& getDeviceInstance() { return deviceInstance; }

        void init(Window& window);
        void shutdown();

        void createImageWithInfo(
            const VkImageCreateInfo& imageInfo,
            const VkMemoryPropertyFlags properties,
            VkImage& image,
            VkDeviceMemory& imageMemory) const;
        VkFormat findSupportedFormat(
            const std::vector<VkFormat>& candidates,
            const VkImageTiling tiling,
            VkFormatFeatureFlags features) const;
        void createBuffer(
            const VkDeviceSize size,
            const VkBufferUsageFlags usage,
            const VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory) const;
        uint32_t findMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags properties) const;

        VkDevice getDevice() const { return device; }
        VkInstance getInstance() const { return instance; }
        VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
        VkSurfaceKHR getSurface() const { return surface; }
        VkQueue getGraphicsQueue() const { return graphicsQueue; }
        VkQueue getPresentQueue() const { return presentQueue; }
        VkCommandPool getCommandPool() const { return commandPool; }
        
        const Utils::SwapChainSupportDetails& getSwapChainSupportDetails() const
        { return swapChainSupportDetails; }
        Utils::QueueFamilyIndices getQueueFamilyIndices() const { return queueFamilyIndices; }

        friend static Device createDeviceInstance();
        friend class Swapchain;
    private:
        Device() = default;

        void createInstance();
        void createSurface(Window& window);
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();

        void validateRequiredInstanceExtensions() const;
        bool isDeviceSuitable(VkPhysicalDevice phyDev);
        Utils::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice phyDev) const;
        Utils::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice phyDev);

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

        static Device deviceInstance;
    };

    static Device createDeviceInstance();

    #define DeviceInstance ::RT::Vulkan::Device::getDeviceInstance()

}
