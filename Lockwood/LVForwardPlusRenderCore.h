#pragma once

#include <vector>
#include <memory>

#include "LVWrapper.h"
#include "LUtility.h"

#include <vulkan\vulkan.h>

class LVContext;
struct GLFWwindow;

#define ADD_LIGHT()
#define ADD_TEXTURE()
#define ADD_MODEL()

class LVForwardPlusRenderCore {
public:
	LVForwardPlusRenderCore(GLFWwindow *_window);
	~LVForwardPlusRenderCore();

	void Draw();
	void Update();
	void Cleanup();

	void RegisterCallbackGUI(std::function<void()> _callback);
	void SetClearColor(float _r, float _g, float _b);

private:
	std::unique_ptr<LVContext> m_VContext;
	std::function<void()> m_GuiCallback;

	LVWrapper<VkSwapchainKHR>				  m_VSwapChain;
	std::vector<LVWrapper<VkImageView>>		  m_VSwapChainImageViews;
	std::vector<VkImage>					  m_VSwapChainImages;
	VkFormat								  m_VSwapChainImageFormat;
	VkExtent2D								  m_VSwapChainExtent;
	LVWrapper<VkRenderPass>					  m_VRenderPass;
	VkDescriptorSet							  m_VDescriptorSet;
	LVWrapper<VkDescriptorSetLayout>		  m_VDescriptorSetLayout;
	LVWrapper<VkDescriptorPool>				  m_VDecriptorPool;
	LVWrapper<VkPipelineLayout>				  m_VGraphicsPipelineLayout;
	LVWrapper<VkPipeline>					  m_VGraphicsPipeline;
	std::vector<LVWrapper<VkFramebuffer>>	  m_VSwapChainFramebuffers;
	LVWrapper<VkSampler>					  m_VTextureSampler;

	LVWrapper<VkBuffer>						  m_UniformStagingBuffer;
	LVWrapper<VkDeviceMemory>				  m_UniformStagingBufferMemory;
	LVWrapper<VkBuffer>						  m_UniformBuffer;
	LVWrapper<VkDeviceMemory>				  m_UniformBufferMemory;
	LVWrapper<VkBuffer>						  m_VertexBuffer;
	LVWrapper<VkDeviceMemory>				  m_VertexBufferMemory;
	LVWrapper<VkBuffer>						  m_IndexBuffer;
	LVWrapper<VkDeviceMemory>				  m_IndexBufferMemory;
						
	std::vector<VkCommandBuffer>			  m_VCommandBuffers;
	std::vector<LVWrapper<VkFence>>			  m_VRenderFences;
	std::vector<LVWrapper<VkCommandPool>>	  m_VRenderCommandPools;
	LVWrapper<VkSemaphore>					  m_VImageAvailableSemaphore;
	LVWrapper<VkSemaphore>					  m_VRenderFinishedSemaphore;
											  
	LVWrapper<VkImage>						  m_VDepthImage;
	LVWrapper<VkDeviceMemory>				  m_VDepthImageMemory;
	LVWrapper<VkImageView>					  m_VDepthImageView;

	LVWrapper<VkImage>						  m_TextureImage;
	LVWrapper<VkDeviceMemory>				  m_TextureImageMemory;
	LVWrapper<VkImageView>					  m_TextureImageView;

	//We support only one batch of geometry as of now.
	std::vector<LUtility::Vertex>			  m_Vertices;
	std::vector<uint32_t>				      m_Indices;

	void CreateSwapChain();
	void CreateSwapChainImageViews();
	void CreateRenderPasses();
	void CreateDescriptorSetLayout();
	void CreateGraphicsPipeline();
	void CreateDepthResources();
	void CreateFramebuffers();
	void CreateTextureSampler();
	void CreateTexture();
	void CreateUniformBuffer();
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateDescriptorPool();
	void CreateDescriptorSet();
	void CreateCommandBuffers();
	void CreateSemaphores();

	void RecordCommandBuffers(int i);
	void BindGUI();

	//Temp
	float m_ClearR;
	float m_ClearG;
	float m_ClearB;
};