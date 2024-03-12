#include "Device.h"
#include <Engine/Core/Assert.h>

#include "utils/Debug.h"

namespace RT::Vulkan
{

	void Device::createInstance()
	{
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "RT Vulkan";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "RT Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        auto debugCreateInfo = populateDebugMessengerCreateInfo();
        enableDebugingForCreateInfo(createInfo, debugCreateInfo);

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            RT_ASSERT(false, "{}", "Could not create Vulkan instance");
        }
	}

}
