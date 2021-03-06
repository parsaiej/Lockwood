#pragma once
#include "LVWrapper.h"

#include <iostream>
#include <vector>
#include <array>
#include <set>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <unordered_map>
#include <chrono>

#define GLFW_INCLUDE_VULKAN
#include "GLFW\glfw3.h"

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\hash.hpp>

namespace LUtility {
	const int ScreenWidth = 1280;
	const int ScreenHeight = 720;
	const glm::vec3 WorldUp = glm::vec3(0, 1, 0);

	const std::vector<const char*> VulkanExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	const std::vector<const char*> ValidationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
		double time;
	};

	struct Vertex {
		glm::vec3 position;
		glm::vec2 texCoord;
		glm::vec3 color;
		glm::vec3 tangent;

		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, color);

			attributeDescriptions[3].binding = 0;
			attributeDescriptions[3].location = 3;
			attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[3].offset = offsetof(Vertex, tangent);

			return attributeDescriptions;
		}

		//Equality operator for hash maps
		bool operator==(const Vertex &other) const {
			return position == other.position && texCoord == other.texCoord && color == other.color && tangent == other.tangent;
		}
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

	QueueFamilyIndices		FindQueueIndices(VkPhysicalDevice _device, VkSurfaceKHR _surface);
	SwapChainSupportDetails QuerySwapChainSupport( VkPhysicalDevice _device, VkSurfaceKHR _surface);
	VkSurfaceFormatKHR		ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &_availableFormats);
	VkPresentModeKHR		ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> _availablePresentModes);
	VkExtent2D				ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &_capabilities, std::array<int, 2> _windowFramebufferSize);
	VkFormat				FindDepthFormat(const VkPhysicalDevice &_device);
	VkFormat				FindSupportedFormat(const std::vector<VkFormat> &_candidates, VkImageTiling _tiling, VkFormatFeatureFlags _features, const VkPhysicalDevice &_device);
	uint32_t				FindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _properties, const VkPhysicalDevice &_physicalDevice);
	std::vector<char>		ReadFile(const std::string &_filePath);
	bool					CheckDeviceExtensionSupport(VkPhysicalDevice _device);
	bool					IsDeviceSuitable(VkPhysicalDevice _device, VkSurfaceKHR _surface);
	bool					CheckValidationLayerSupport();
	inline bool				HasStencilComponent(VkFormat _format) { return _format == VK_FORMAT_D32_SFLOAT_S8_UINT || _format == VK_FORMAT_D24_UNORM_S8_UINT; }
	void					LoadModel(const std::string &_filePath, std::vector<Vertex> &_vertices, std::vector<uint32_t> &_indices);
	void					CreateBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, LVWrapper<VkBuffer> &_buffer, LVWrapper<VkDeviceMemory> &_bufferMemory, const VkDevice &_device, const VkPhysicalDevice &_physicalDevice);
	void					CopyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size, const VkDevice &_device, const VkCommandPool &_cmdPool, const VkQueue &_gfxQueue);
	void					CopyImage(VkImage _srcImage, VkImage _dstImage, uint32_t _width, uint32_t _height, const VkDevice &_device, const VkQueue &_gfxQueue, const VkCommandPool &_cmdPool);
	void					CreateImage(uint32_t _width, uint32_t _height, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties, LVWrapper<VkImage> &_image, LVWrapper<VkDeviceMemory> &_imageMemory, const VkDevice &_device, const VkPhysicalDevice &_physicalDevice);
	void					CreateImageView(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags, LVWrapper<VkImageView> &_imageView, const VkDevice &_device);
	void					CreateTextureImage(const std::string &_filePath, LVWrapper<VkImage> &_stagingImage, LVWrapper<VkDeviceMemory> &_stagingImageMemory, LVWrapper<VkImage> &_image, LVWrapper<VkDeviceMemory> &_imageMemory, const VkDevice &_device, const VkPhysicalDevice &_physicalDevice, const VkQueue &_gfxQueue, const VkCommandPool &_cmdPool);
	void					CreateTextureImageView(VkImage _image, LVWrapper<VkImageView> &_imageView, const VkDevice &_device);
	void					CreateShaderModule(const std::vector<char>& _code, LVWrapper<VkShaderModule> &_shaderModule, const VkDevice &_device);
	void					TransitionImageLayout(VkImage _image, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, const VkQueue &_gfxQueue, const VkCommandPool &_cmdPool, const VkDevice &_device);

	VkCommandBuffer BeginSingleTimeCommands(const VkCommandPool &_cmdPool, const VkDevice &_device);
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue _gfxQueue, const VkCommandPool &_cmdPool, const VkDevice &_device);

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

	inline void Log(const char *_msg) {
		std::string s = "[Lockwood Core] CONSOLE: ";
		s += _msg;
		printf("%s\n", s.c_str());
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

//Custom hash operator for Vertex struct.
namespace std {
	template<> struct hash<LUtility::Vertex> {
		size_t operator()(LUtility::Vertex const& _vertex) const {
			return ((hash<glm::vec3>()(_vertex.position) ^ (hash<glm::vec3>()(_vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(_vertex.texCoord) << 1);
		}
	};
}