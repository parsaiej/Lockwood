#include "..\LUtility.h"
#include <GLFW\glfw3.h>

VkCommandBuffer LUtility::BeginSingleTimeCommands(const VkCommandPool &_cmdPool, const VkDevice &_device) {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = _cmdPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	return commandBuffer;
}

void LUtility::EndSingleTimeCommands(VkCommandBuffer _commandBuffer, VkQueue _gfxQueue, const VkCommandPool &_cmdPool, const VkDevice &_device) {
	vkEndCommandBuffer(_commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_commandBuffer;

	vkQueueSubmit(_gfxQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(_gfxQueue);
	vkFreeCommandBuffers(_device, _cmdPool, 1, &_commandBuffer);
}

LUtility::QueueFamilyIndices LUtility::FindQueueIndices(VkPhysicalDevice _device, VkSurfaceKHR _surface) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, _surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}

		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		if (indices.isComplete()) break;
		i++;
	}

	return indices;
}

LUtility::SwapChainSupportDetails LUtility::QuerySwapChainSupport(VkPhysicalDevice _device, VkSurfaceKHR _surface) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device, _surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(_device, _surface, &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(_device, _surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(_device, _surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(_device, _surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR LUtility::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &_availableFormats) {

	if (_availableFormats.size() == 1 && _availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : _availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return _availableFormats[0];
}

VkPresentModeKHR LUtility::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> _availablePresentModes) {
	for (const auto& availablePresentMode : _availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D LUtility::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &_capabilities, std::array<int, 2> _windowFramebufferSize)
{
	if (_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return _capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { (uint32_t)_windowFramebufferSize[0], (uint32_t)_windowFramebufferSize[1] };

		actualExtent.width = std::max(_capabilities.minImageExtent.width, std::min(_capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(_capabilities.minImageExtent.height, std::min(_capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VkFormat LUtility::FindSupportedFormat(const std::vector<VkFormat> &_candidates, 
									   VkImageTiling _tiling, 
									   VkFormatFeatureFlags _features,
									   const VkPhysicalDevice &_physicalDevice) {
	for (VkFormat format : _candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &props);

		if (_tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & _features) == _features) {
			return format;
		}
		else if (_tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & _features) == _features) {
			return format;
		}
	}
	RuntimeError("Failed To Find Supported Format.");
}

VkFormat LUtility::FindDepthFormat(const VkPhysicalDevice &_physicalDevice) {
	return FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
								 VK_IMAGE_TILING_OPTIMAL,
								 VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
								_physicalDevice);
}

uint32_t LUtility::FindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _properties, const VkPhysicalDevice &_physicalDevice) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if (_typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & _properties) == _properties) {
			return i;
		}
	}
	RuntimeError("Failed To Find Suitable Memory Type");
	return -1;
}

std::vector<char> LUtility::ReadFile(const std::string &_filepath) {
	std::ifstream file(_filepath, std::ios::ate | std::ios::binary);
	if (!file.is_open()) RuntimeError("Failed To Open File.");

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

bool LUtility::CheckDeviceExtensionSupport(VkPhysicalDevice _device) {

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(VulkanExtensions.begin(), VulkanExtensions.end());
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool LUtility::IsDeviceSuitable(VkPhysicalDevice _device, VkSurfaceKHR _surface) {
	QueueFamilyIndices indices = FindQueueIndices(_device, _surface);
	bool extensionsSupported = CheckDeviceExtensionSupport(_device);
	
	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_device, _surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool LUtility::CheckValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : LUtility::ValidationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) return false;
	}

	return true;
}

void LUtility::CreateBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, 
				LVWrapper<VkBuffer> &_buffer, LVWrapper<VkDeviceMemory> &_bufferMemory, 
				const VkDevice &_device, const VkPhysicalDevice &_physicalDevice) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = _size;
	bufferInfo.usage = _usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateBuffer(_device, &bufferInfo, nullptr, _buffer.Replace()) != VK_SUCCESS)
		RuntimeError("Failed To Create Buffer.");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(_device, _buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _physicalDevice);

	if (vkAllocateMemory(_device, &allocInfo, nullptr, _bufferMemory.Replace()) != VK_SUCCESS)
		RuntimeError("Failed To Allocate Buffer Memory.");
	vkBindBufferMemory(_device, _buffer, _bufferMemory, 0);
}

void LUtility::CreateImage(uint32_t _width, uint32_t _height, 
						   VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties, 
						   LVWrapper<VkImage> &_image, LVWrapper<VkDeviceMemory> &_imageMemory, 
						   const VkDevice &_device, const VkPhysicalDevice &_physicalDevice) {

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = _width;
	imageInfo.extent.height = _height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = _format;
	imageInfo.tiling = _tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageInfo.usage = _usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;
	if (vkCreateImage(_device, &imageInfo, nullptr, _image.Replace()) != VK_SUCCESS)
		RuntimeError("Failed To Create Image.");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(_device, _image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, _properties, _physicalDevice);
	if (vkAllocateMemory(_device, &allocInfo, nullptr, _imageMemory.Replace()) != VK_SUCCESS)
		RuntimeError("Failed To Allocate Image Memory.");
	vkBindImageMemory(_device, _image, _imageMemory, 0);

}

void LUtility::CreateImageView(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags, LVWrapper<VkImageView> &_imageView, const VkDevice &_device) {
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = _image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = _format;
	viewInfo.subresourceRange.aspectMask = _aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(_device, &viewInfo, nullptr, _imageView.Replace()) != VK_SUCCESS)
	    RuntimeError("Failed To Create Texture Image View.");
}

void LUtility::CreateShaderModule(const std::vector<char>& _code, LVWrapper<VkShaderModule> &_shaderModule, const VkDevice &_device) {
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = _code.size();
	createInfo.pCode = (uint32_t*)_code.data();

	if (vkCreateShaderModule(_device, &createInfo, nullptr, _shaderModule.Replace()) != VK_SUCCESS)
		RuntimeError("Failed To Create Shader Module.");
}

//TODO: Make less shit
void LUtility::TransitionImageLayout(VkImage _image, VkFormat _format, 
									 VkImageLayout _oldLayout, VkImageLayout _newLayout,
									 const VkQueue &_gfxQueue, const VkCommandPool &_cmdPool, const VkDevice &_device) {
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands(_cmdPool, _device);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = _oldLayout;
	barrier.newLayout = _newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = _image;

	if (_newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (HasStencilComponent(_format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	if (_oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else {
		RuntimeError("Unsupported Layout Transition.");
	}

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	EndSingleTimeCommands(commandBuffer, _gfxQueue, _cmdPool, _device);
}