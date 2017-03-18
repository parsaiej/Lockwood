#pragma once

#include <memory>

#include "LVWrapper.h"

#include <vulkan\vulkan.h>

struct GLFWwindow;

class LVContext {
public:
	LVContext(GLFWwindow *_window);
	~LVContext() {};

	std::array<int, 2> WindowFramebufferSize();

	//Vulkan Context Handle Getter Interface
	//-----------------------------------------------------------------------------
	inline GLFWwindow*						  GetWindowHandle()			  { return m_glfwWindowHandle; }
	inline const LVWrapper<VkDevice>&		  GetDevice()		    const { return m_VDevice; }
	inline const LVWrapper<VkSurfaceKHR>&     GetSurface()          const { return m_VSurface;  }
	inline const VkPhysicalDevice&			  GetPhysicalDevice()   const { return m_VPhysicalDevice; }
	inline const LVWrapper<VkCommandPool>&	  GetCommandPool()	    const { return m_VCommandPool; }
	inline const VkQueue&		              GetGraphicsQueue()    const { return m_VGraphicsQueue; }
	inline const VkQueue&		  			  GetPresentQueue()	    const { return m_VPresentQueue; }
	//-----------------------------------------------------------------------------

private:
	GLFWwindow *m_glfwWindowHandle;

	//Vulkan Context Handles
	//------------------------------------------------
	LVWrapper<VkInstance>				m_VInstance;
	LVWrapper<VkDebugReportCallbackEXT> m_VDebugValidationLayerCallback;
	LVWrapper<VkSurfaceKHR>				m_VSurface;

	VkPhysicalDevice					m_VPhysicalDevice;
	VkQueue m_VGraphicsQueue;
	VkQueue m_VPresentQueue;

	LVWrapper<VkDevice>					m_VDevice;
	LVWrapper<VkCommandPool>			m_VCommandPool;
	//------------------------------------------------

	void CreateInstance		();
	void SetupDebugCallback ();
	void CreateSurface		();
	void PickPhysicalDevice ();
	void CreateLogicalDevice();
	void CreateCommandPool	();
};