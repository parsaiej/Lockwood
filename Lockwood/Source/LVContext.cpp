#include "..\LVContext.h"
#include "..\LUtility.h"

#include <GLFW\glfw3.h>

#ifdef NDEBUG
#define VALIDATION_LAYERS 0
#else
#define VALIDATION_LAYERS 1
#endif

LVContext::LVContext(GLFWwindow *_window)
	: m_glfwWindowHandle(_window),
	m_VInstance{ vkDestroyInstance },
	m_VDebugValidationLayerCallback{ m_VInstance, LUtility::DestroyDebugReportCallbackEXT },
	m_VSurface{ m_VInstance, vkDestroySurfaceKHR },
	m_VPhysicalDevice(VK_NULL_HANDLE),
	m_VDevice{ vkDestroyDevice },
	m_VCommandPool { m_VDevice, vkDestroyCommandPool }
{
	CreateInstance();
	SetupDebugCallback();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateCommandPool();
}

std::vector<const char *> GetRequiredExtensions() {
	std::vector<const char*> extensions;

	unsigned int glfwExtensionsCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

	for (unsigned int i = 0; i < glfwExtensionsCount; ++i) {
		extensions.push_back(glfwExtensions[i]);
	}
	if (VALIDATION_LAYERS) extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	return extensions;
}

std::array<int, 2> LVContext::WindowFramebufferSize() {
	int w, h;
	glfwGetFramebufferSize(m_glfwWindowHandle, &w, &h);
	return std::array<int, 2> {w, h};
}

void LVContext::CreateInstance() {
	if (VALIDATION_LAYERS && !LUtility::CheckValidationLayerSupport())
		LUtility::RuntimeError("Validation Layers Requested, But Unavailable.");

	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "Forward+ Demo";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "Lockwood";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

	std::vector<const char*> extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount	 = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames   = extensions.data();

	if (VALIDATION_LAYERS) {
		createInfo.enabledLayerCount   = static_cast<uint32_t>(LUtility::ValidationLayers.size());
		createInfo.ppEnabledLayerNames = LUtility::ValidationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, m_VInstance.Replace()) != VK_SUCCESS) 
		LUtility::RuntimeError("Failed To Create Vulkan Instance.");
}

void LVContext::SetupDebugCallback() {
	if (!VALIDATION_LAYERS) return;

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = LUtility::DebugValidationLayerCallback;

	if (LUtility::CreateDebugReportCallbackEXT(m_VInstance, &createInfo, nullptr, m_VDebugValidationLayerCallback.Replace()) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Set Up Debug Callback.");
}

void LVContext::CreateSurface() {
	if (glfwCreateWindowSurface(m_VInstance, m_glfwWindowHandle, nullptr, m_VSurface.Replace()) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Create Window Surface.");
}

void LVContext::PickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_VInstance, &deviceCount, nullptr);

	if (deviceCount == 0) LUtility::RuntimeError("Failed To Find GPU(s) with Vulkan Support.");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_VInstance, &deviceCount, devices.data());
	for (const auto& device : devices) {
		if (LUtility::IsDeviceSuitable(device, m_VSurface)) {
			m_VPhysicalDevice = device;
			break;
		}
	}
	if (m_VPhysicalDevice == VK_NULL_HANDLE) LUtility::RuntimeError("Failed To Find Suitable GPU.");

	VkPhysicalDeviceProperties prop;
	vkGetPhysicalDeviceProperties(m_VPhysicalDevice, &prop);
	LUtility::Log(prop.deviceName);
}

void LVContext::CreateLogicalDevice() {

	LUtility::QueueFamilyIndices indices = LUtility::FindQueueIndices(m_VPhysicalDevice, m_VSurface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };
	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createinfo = {};
	createinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createinfo.pQueueCreateInfos = queueCreateInfos.data();
	createinfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
	createinfo.pEnabledFeatures = &deviceFeatures;
	createinfo.enabledExtensionCount = static_cast<uint32_t>(LUtility::VulkanExtensions.size());
	createinfo.ppEnabledExtensionNames = LUtility::VulkanExtensions.data();

	if (VALIDATION_LAYERS) {
		createinfo.enabledLayerCount = static_cast<uint32_t>(LUtility::ValidationLayers.size());
		createinfo.ppEnabledLayerNames = LUtility::ValidationLayers.data();
	}
	else {
		createinfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_VPhysicalDevice, &createinfo, nullptr, m_VDevice.Replace()) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Create Logical Device.");

	vkGetDeviceQueue(m_VDevice, indices.graphicsFamily, 0, &m_VGraphicsQueue);
	vkGetDeviceQueue(m_VDevice, indices.presentFamily,  0, &m_VPresentQueue);
}

void LVContext::CreateCommandPool() {
	LUtility::QueueFamilyIndices indices = LUtility::FindQueueIndices(m_VPhysicalDevice, m_VSurface);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamily;
	poolInfo.flags = 0;
	
	if (vkCreateCommandPool(m_VDevice, &poolInfo, nullptr, m_VCommandPool.Replace()) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Create Command Pool.");
}