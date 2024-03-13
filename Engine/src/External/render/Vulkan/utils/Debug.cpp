#include "Debug.h"
#include <Engine/Core/Log.h>

#include "External/window/GlfwWindow/Utils.h"

namespace RT::Vulkan
{

	VkDebugUtilsMessengerEXT debugMessenger{};
	
	bool checkValidationLayerSupport()
	{
		uint32_t layerCount = 0u;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		auto availableLayers = std::vector<VkLayerProperties>(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : ValidationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> getRequiredExtensions()
	{
		auto extensions = Glfw::getInstanceExtensions();

		if (EnableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	VkResult createDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto createValidation = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance,
			"vkCreateDebugUtilsMessengerEXT");
		return createValidation ?
			createValidation(instance, pCreateInfo, pAllocator, pDebugMessenger) :
			VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void destroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		if constexpr (EnableValidationLayers)
		{
			auto destroyValidation = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
				instance,
				"vkDestroyDebugUtilsMessengerEXT");
			if (destroyValidation != nullptr)
			{
				destroyValidation(instance, debugMessenger, pAllocator);
			}
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		RT_LOG_INFO("[Vulkan] ERR: {}", pCallbackData->pMessage);
		return VK_FALSE;
	}

	VkDebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo()
	{
		auto createInfo = VkDebugUtilsMessengerCreateInfoEXT{};
		if constexpr (EnableValidationLayers)
		{
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = debugCallback;
			createInfo.pUserData = nullptr;
		}
		return createInfo;
	}

	void enableDebugingForCreateInfo(VkInstanceCreateInfo& createInfo, VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo)
	{
		if constexpr (EnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
			createInfo.ppEnabledLayerNames = ValidationLayers.data();
			createInfo.pNext = &debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}
	}

	void setupDebugMessenger(VkInstance& instance)
	{
		if constexpr (EnableValidationLayers)
		{
			auto createInfo = populateDebugMessengerCreateInfo();
			if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to set up debug messenger!");
			}
		}
	}

}
