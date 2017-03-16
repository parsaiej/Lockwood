#pragma once

#include <iostream>
#include <cstring>
#include <vector>
#include <set>
#include <vulkan\vulkan.h>

namespace LUtility {

	const std::vector<const char*> VulkanExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	const std::vector<const char*> ValidationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	struct QueueFamilyIndices {
		int graphicsFamily = -1;
		int presentFamily = -1;

		bool isComplete() {
			return graphicsFamily >= 0 && presentFamily >= 0;
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR		capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR>	presentModes;
	};

	
	QueueFamilyIndices FindQueueIndices(VkPhysicalDevice _device, VkSurfaceKHR _surface);
	SwapChainSupportDetails QuerySwapChainSupport( VkPhysicalDevice _device, VkSurfaceKHR _surface);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice _device);
	bool IsDeviceSuitable(VkPhysicalDevice _device, VkSurfaceKHR _surface);
	bool CheckValidationLayerSupport();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugValidationLayerCallback(VkDebugReportFlagsEXT flags,
																	   VkDebugReportObjectTypeEXT objType,
																	   uint64_t obj,
																	   size_t location,
																	   int32_t code,
																	   const char* layerPrefix,
																	   const char* msg,
																	   void* userData) {
		std::cerr << "[Lockwood Core] Validation Layer: " << msg << std::endl;
		return VK_FALSE;
	}


	inline void RuntimeError(const char *_msg) {
		std::string s = "[Lockwood Core] ERROR: ";
		s += _msg;
		throw std::runtime_error(s.c_str());
	}

	inline VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
		auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pCallback);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	inline void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
		if (func != nullptr) {
			func(instance, callback, pAllocator);
		}
	}
}