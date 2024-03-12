#pragma once
#include <vulkan/vulkan.h>

namespace RT::Vulkan
{

    class Window;

	class Device
	{
	public:

    private:
        void createInstance();

	private:
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        Window& window;
        VkCommandPool commandPool;

        VkDevice device;
        VkSurfaceKHR surface;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
	};

}
