#pragma once
#include <array>
#include <vector>

#include <vulkan/vulkan.h>

#include "External/window/GlfwWindow/GlfwWindow.h"

namespace RT::Vulkan
{

    struct SwapChainSupportDetails;

    struct QueueFamilyIndices
    {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
    };

    class Device
    {
    public:
        Device(Window* window);
        ~Device();
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();
        bool checkValidationLayerSupport();
        std::vector<const char*> getRequiredExtensions();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void hasGflwRequiredInstanceExtensions();
        bool isDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        VkFormat findSupportedFormat(
            const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        void createImageWithInfo(
            const VkImageCreateInfo& imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage& image,
            VkDeviceMemory& imageMemory);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    //private:
        VkInstance instance{};
        VkDebugUtilsMessengerEXT debugMessenger{};
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        Window* window;
        VkCommandPool commandPool{};

        VkDevice device;
        VkSurfaceKHR surface;
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        VkPhysicalDeviceProperties properties;

        static constexpr std::array<const char*, 1> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        static constexpr std::array<const char*, 1> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        static constexpr bool enableValidationLayers = true;
    };

}
