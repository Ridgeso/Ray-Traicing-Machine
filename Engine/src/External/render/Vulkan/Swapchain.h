#pragma once
#include <vector>
#include <vulkan/vulkan.h>

#include "Device.h"
#include "RenderPass.h"
#include "VulkanFrameBuffer.h"
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
        bool compareFormats(const Swapchain& other) const;
        static VkFormat findDepthFormat();

        VkSwapchainKHR getSwapChain() const { return swapChain; }
        VkRenderPass getRenderPass() const { return renderPass; }
        const std::vector<VulkanFrameBuffer>& getFramebuffers() const { return framebuffers; }
        VkExtent2D getWindowExtent() const { return windowExtent; }
        VkExtent2D getSwapchainExtent() const { return swapChainExtent; }
        const std::vector<VkImage>& getSwapChainImages() const { return swapChainImages; }
        const std::vector<VkImageView>& getSwapChainImageViews() const { return swapChainImageViews; }
        VkFormat getImageFormat() const { return swapChainImageFormat; }

        static constexpr uint32_t minImageCount() { return 2; }

    private:
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createDepthResources();
        void createFramebuffers();
        void createSyncObjects();

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
        bool compareSwapFormats(const Swapchain& swapChain) const;
        void incrementFrameCounter();
    
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>& availableFormats);
        static VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR>& availablePresentModes);

    private:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        
        VkSwapchainKHR swapChain = {};
        RenderPass renderPassNew = {};
        VkRenderPass renderPass = {};
        std::vector<VulkanFrameBuffer> framebuffers = {};
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
