#include "..\LVForwardPlusRenderCore.h"
#include "..\LVContext.h"
#include "..\LUtility.h"

LVForwardPlusRenderCore::LVForwardPlusRenderCore(GLFWwindow *_window) 
	: m_VContext(std::make_unique<LVContext>(_window)),
	m_VSwapChain				  { m_VContext->GetDevice(), vkDestroySwapchainKHR},
	m_VRenderPass				  { m_VContext->GetDevice(), vkDestroyRenderPass },
	m_VDescriptorSetLayout		  { m_VContext->GetDevice(), vkDestroyDescriptorSetLayout },
	m_VGraphicsPipelineLayout	  { m_VContext->GetDevice(), vkDestroyPipelineLayout },
	m_VGraphicsPipeline			  { m_VContext->GetDevice(), vkDestroyPipeline },
	m_VDepthImage				  { m_VContext->GetDevice(), vkDestroyImage },
	m_VDepthImageMemory			  { m_VContext->GetDevice(), vkFreeMemory },
	m_VDepthImageView			  { m_VContext->GetDevice(), vkDestroyImageView },
	m_VTextureSampler			  { m_VContext->GetDevice(), vkDestroySampler},
	m_UniformStagingBuffer		  { m_VContext->GetDevice(), vkDestroyBuffer },
	m_UniformStagingBufferMemory  { m_VContext->GetDevice(), vkFreeMemory },
	m_UniformBuffer				  { m_VContext->GetDevice(), vkDestroyBuffer },
	m_UniformBufferMemory		  { m_VContext->GetDevice(), vkFreeMemory }
{
	LUtility::Log("Vulkan Initialized.");
	this->CreateSwapChain();
	this->CreateSwapChainImageViews();
	this->CreateRenderPasses();
	this->CreateDescriptorSetLayout();
	this->CreateGraphicsPipeline();
	this->CreateDepthResources();
	this->CreateFramebuffers();
	this->CreateTextureSampler();
	this->CreateUniformBuffer();
}

LVForwardPlusRenderCore::~LVForwardPlusRenderCore() {}

void LVForwardPlusRenderCore::CreateSwapChain() {
	LUtility::SwapChainSupportDetails swapChainSupport = LUtility::QuerySwapChainSupport(m_VContext->GetPhysicalDevice(), m_VContext->GetSurface());

	VkSurfaceFormatKHR surfaceFormat = LUtility::ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode	 = LUtility::ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent				 = LUtility::ChooseSwapExtent(swapChainSupport.capabilities, m_VContext->WindowFramebufferSize());

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_VContext->GetSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	LUtility::QueueFamilyIndices indices = LUtility::FindQueueIndices(m_VContext->GetPhysicalDevice(), m_VContext->GetSurface());
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	VkSwapchainKHR oldSwapChain = m_VSwapChain;
	createInfo.oldSwapchain = oldSwapChain;

	VkSwapchainKHR newSwapChain;
	if (vkCreateSwapchainKHR(m_VContext->GetDevice(), &createInfo, nullptr, &newSwapChain) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Create Swap Chain.");
	m_VSwapChain = newSwapChain;
	
	vkGetSwapchainImagesKHR(m_VContext->GetDevice(), m_VSwapChain, &imageCount, nullptr);
	m_VSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_VContext->GetDevice(), m_VSwapChain, &imageCount, m_VSwapChainImages.data());

	m_VSwapChainImageFormat = surfaceFormat.format;
	m_VSwapChainExtent = extent;
}

void LVForwardPlusRenderCore::CreateSwapChainImageViews() {
	m_VSwapChainImageViews.resize(m_VSwapChainImages.size(), LVWrapper<VkImageView>{m_VContext->GetDevice(), vkDestroyImageView});
	for (uint32_t i = 0; i < m_VSwapChainImageViews.size(); i++) {
		LUtility::CreateImageView(m_VSwapChainImages[i], m_VSwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, m_VSwapChainImageViews[i], m_VContext->GetDevice());
	}
}

void LVForwardPlusRenderCore::CreateRenderPasses() {
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = m_VSwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = LUtility::FindDepthFormat(m_VContext->GetPhysicalDevice());
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	if (vkCreateRenderPass(m_VContext->GetDevice(), &renderPassInfo, nullptr, m_VRenderPass.Replace()) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Create Render Pass.");
}

void LVForwardPlusRenderCore::CreateDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
	if (vkCreateDescriptorSetLayout(m_VContext->GetDevice(), &layoutInfo, nullptr, m_VDescriptorSetLayout.Replace()) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Create Descriptor Set Layout.");
}

void LVForwardPlusRenderCore::CreateGraphicsPipeline() {
	auto vertShaderCode = LUtility::ReadFile("shaders/vert.spv");
	auto fragShaderCode = LUtility::ReadFile("shaders/frag.spv");

	LVWrapper<VkShaderModule> vertShaderModule{ m_VContext->GetDevice(), vkDestroyShaderModule };
	LVWrapper<VkShaderModule> fragShaderModule{ m_VContext->GetDevice(), vkDestroyShaderModule };
	LUtility::CreateShaderModule(vertShaderCode, vertShaderModule, m_VContext->GetDevice());
	LUtility::CreateShaderModule(fragShaderCode, fragShaderModule, m_VContext->GetDevice());

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	auto bindingDescription	  = LUtility::Vertex::GetBindingDescription();
	auto attributeDescription = LUtility::Vertex::GetAttributeDescriptions();
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_VSwapChainExtent.width;
	viewport.height = (float)m_VSwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = m_VSwapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	VkDescriptorSetLayout setLayouts[] = { m_VDescriptorSetLayout };
	pipelineLayoutInfo.pSetLayouts = setLayouts;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = 0;
	if (vkCreatePipelineLayout(m_VContext->GetDevice(), &pipelineLayoutInfo, nullptr, m_VGraphicsPipelineLayout.Replace()) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Create Pipeline Layout");

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f;
	depthStencil.maxDepthBounds = 1.0f;
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {};
	depthStencil.back = {};

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = m_VGraphicsPipelineLayout;
	pipelineInfo.renderPass = m_VRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(m_VContext->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, m_VGraphicsPipeline.Replace()) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Create Graphics Pipeline.");
}

void LVForwardPlusRenderCore::CreateDepthResources() {
	VkFormat depthFormat = LUtility::FindDepthFormat(m_VContext->GetPhysicalDevice());
	LUtility::CreateImage(m_VSwapChainExtent.width, m_VSwapChainExtent.height,
						  depthFormat,
						  VK_IMAGE_TILING_OPTIMAL,
						  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
						  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						  m_VDepthImage, m_VDepthImageMemory,
						  m_VContext->GetDevice(), m_VContext->GetPhysicalDevice());
	LUtility::CreateImageView(m_VDepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, m_VDepthImageView, m_VContext->GetDevice());
	LUtility::TransitionImageLayout(m_VDepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
									m_VContext->GetGraphicsQueue(), m_VContext->GetCommandPool(), m_VContext->GetDevice());
}

void LVForwardPlusRenderCore::CreateFramebuffers() {
	m_VSwapChainFramebuffers.resize(m_VSwapChainImageViews.size(), LVWrapper<VkFramebuffer>{m_VContext->GetDevice(), vkDestroyFramebuffer});
	for (size_t i = 0; i < m_VSwapChainImageViews.size(); i++) {
		std::array<VkImageView, 2> attachments[] = {
			m_VSwapChainImageViews[i],
			m_VDepthImageView
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_VRenderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments->size());
		framebufferInfo.pAttachments = attachments->data();
		framebufferInfo.width = m_VSwapChainExtent.width;
		framebufferInfo.height = m_VSwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_VContext->GetDevice(), &framebufferInfo, nullptr, m_VSwapChainFramebuffers[i].Replace()) != VK_SUCCESS)
			LUtility::RuntimeError("Failed To Create Framebuffer.");
	}
}

void LVForwardPlusRenderCore::CreateTextureSampler() {
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	if (vkCreateSampler(m_VContext->GetDevice(), &samplerInfo, nullptr, m_VTextureSampler.Replace()) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Create Texture Sampler.");
}

void LVForwardPlusRenderCore::CreateUniformBuffer() {

	VkDeviceSize bufferSize = sizeof(LUtility::UniformBufferObject);

	LUtility::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		m_UniformStagingBuffer, m_UniformStagingBufferMemory,
		m_VContext->GetDevice(), m_VContext->GetPhysicalDevice());

	LUtility::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_UniformBuffer, m_UniformBufferMemory,
		m_VContext->GetDevice(), m_VContext->GetPhysicalDevice());
}