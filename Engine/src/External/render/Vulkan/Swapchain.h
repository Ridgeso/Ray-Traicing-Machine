#pragma once
#include <vector>
#include <vulkan/vulkan.h>

#include "Device.h"
#include "utils/Utils.h"

namespace RT::Vulkan
{

    class Swapchain
    {
    public:
        Swapchain(const VkExtent2D windowExtent, const Share<Swapchain>& oldSwapchain = nullptr);
        ~Swapchain() = default;

        Swapchain(const Swapchain&) = delete;
        Swapchain(Swapchain&&) = delete;
        Swapchain& operator=(const Swapchain&) = delete;
        Swapchain&& operator=(Swapchain&&) = delete;
        
        void init();
        void shutdown();

        VkResult acquireNextImage(uint32_t& imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer& buffers, uint32_t& imageIndex);

        VkSwapchainKHR getSwapChain() const { return swapChain; }
        VkRenderPass getRenderPass() const { return renderPass; }
        const std::vector<VkFramebuffer>& getSwapChainFramebuffers() const { return swapChainFramebuffers; }
        VkExtent2D getWindowExtent() const { return windowExtent; }
        const std::vector<VkImage> getSwapChainImages() const { return swapChainImages; }

    private:
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createDepthResources();
        void createFramebuffers();
        void createSyncObjects();

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
        VkFormat findDepthFormat();
        bool compareSwapFormats(const Swapchain& swapChain) const;
        void incrementFrameCounter();
    
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>& availableFormats);
        static VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR>& availablePresentModes);

    private:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        
        VkSwapchainKHR swapChain = {};
        VkRenderPass renderPass = {};
        std::vector<VkFramebuffer> swapChainFramebuffers = {};

        VkFormat swapChainImageFormat = {};
        VkFormat swapChainDepthFormat = {};
        VkExtent2D swapChainExtent = {};
        const VkExtent2D windowExtent = {};

        std::vector<VkImage> depthImages = {};
        std::vector<VkDeviceMemory> depthImageMemorys = {};
        std::vector<VkImageView> depthImageViews = {};
        std::vector<VkImage> swapChainImages = {};
        std::vector<VkImageView> swapChainImageViews = {};

        std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphores = {};
        std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> renderFinishedSemaphores = {};
        std::array<VkFence, MAX_FRAMES_IN_FLIGHT> inFlightFences = {};
        std::vector<VkFence> imagesInFlight = {};
        uint8_t currentFrame = 0u;

        Share<Swapchain> oldSwapchain = nullptr;
    };

}
