#pragma once

#include <vector>
#include <memory>

#include "LVWrapper.h"

#include <vulkan\vulkan.h>

class LVContext;
struct GLFWwindow;

class LVForwardPlusRenderCore {
public:
	LVForwardPlusRenderCore(GLFWwindow *_window);
	~LVForwardPlusRenderCore();

	void Draw() {};

private:
	std::unique_ptr<LVContext> m_VContext;

	LVWrapper<VkSwapchainKHR>				m_VSwapChain;
	std::vector<LVWrapper<VkImageView>>		m_VSwapChainImageViews;
	std::vector<VkImage>					m_VSwapChainImages;
	VkFormat								m_VSwapChainImageFormat;
	VkExtent2D								m_VSwapChainExtent;
	LVWrapper<VkRenderPass>					m_VRenderPass;
	LVWrapper<VkDescriptorSetLayout>		m_VDescriptorSetLayout;
	LVWrapper<VkPipelineLayout>				m_VGraphicsPipelineLayout;
	LVWrapper<VkPipeline>					m_VGraphicsPipeline;
	std::vector<LVWrapper<VkFramebuffer>>	m_VSwapChainFramebuffers;
	LVWrapper<VkSampler>					m_VTextureSampler;

	LVWrapper<VkBuffer>		  m_UniformStagingBuffer;
	LVWrapper<VkDeviceMemory> m_UniformStagingBufferMemory;
	LVWrapper<VkBuffer>		  m_UniformBuffer;
	LVWrapper<VkDeviceMemory> m_UniformBufferMemory;

	LVWrapper<VkImage>		  m_VDepthImage;
	LVWrapper<VkDeviceMemory> m_VDepthImageMemory;
	LVWrapper<VkImageView>	  m_VDepthImageView;

	void CreateSwapChain();
	void CreateSwapChainImageViews();
	void CreateRenderPasses();
	void CreateDescriptorSetLayout();
	void CreateGraphicsPipeline();
	void CreateDepthResources();
	void CreateFramebuffers();
	void CreateTextureSampler();
	void CreateUniformBuffer();
};