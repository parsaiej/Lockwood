#include "..\LVForwardPlusRenderCore.h"
#include "..\LVContext.h"
#include "..\LUtility.h"
#include "..\LVGui.h"

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
	m_UniformBufferMemory		  { m_VContext->GetDevice(), vkFreeMemory },
	m_VertexBuffer				  { m_VContext->GetDevice(), vkDestroyBuffer },
	m_VertexBufferMemory		  { m_VContext->GetDevice(), vkFreeMemory },
	m_IndexBuffer				  { m_VContext->GetDevice(), vkDestroyBuffer },
	m_IndexBufferMemory			  { m_VContext->GetDevice(), vkFreeMemory },

	//m_TextureImage				  { m_VContext->GetDevice(), vkDestroyImage },
	//m_TextureImageMemory		      { m_VContext->GetDevice(), vkFreeMemory },
	//m_TextureImageView			  { m_VContext->GetDevice(), vkDestroyImageView },

	m_VDecriptorPool			  { m_VContext->GetDevice(), vkDestroyDescriptorPool },
	m_VDescriptorSet(VK_NULL_HANDLE),
	m_VImageAvailableSemaphore	  { m_VContext->GetDevice(), vkDestroySemaphore },
	m_VRenderFinishedSemaphore	  { m_VContext->GetDevice(), vkDestroySemaphore }
{
	LUtility::Log("Vulkan Context Created.");
	this->CreateSwapChain();			LUtility::Log("Swap Chain Created");
	this->CreateSwapChainImageViews();	LUtility::Log("Swap Chain Image Views Created");
	this->CreateRenderPasses();			LUtility::Log("Render Passes Created");
	this->CreateDescriptorSetLayout();	LUtility::Log("Descriptor Set Layout Created.");
	this->CreateGraphicsPipeline();		LUtility::Log("Graphics Pipeline Created.");
	this->CreateDepthResources();		LUtility::Log("Depth Resources Created");
	this->CreateFramebuffers();			LUtility::Log("Framebuffers Created.");
	this->CreateTextureSampler();		LUtility::Log("Texture Sampler Created.");
	this->CreateUniformBuffer();		LUtility::Log("UBO Created.");

	LUtility::LoadModel("../Assets/models/Sphere.obj", m_Vertices, m_Indices);
	this->CreateVertexBuffer();
	this->CreateIndexBuffer();

	m_TextureSet = 0;
	m_TextureImages.       resize(30, LVWrapper<VkImage>       {m_VContext->GetDevice(), vkDestroyImage});
	m_TextureImageMemories.resize(30, LVWrapper<VkDeviceMemory>{m_VContext->GetDevice(), vkFreeMemory});
	m_TextureImageViews.   resize(30, LVWrapper<VkImageView>   {m_VContext->GetDevice(), vkDestroyImageView});
	this->CreateTexture("../Assets/textures/sphere/whiteTile/normals.png",    0);	
	this->CreateTexture("../Assets/textures/sphere/whiteTile/diffuse.png",    1);
	this->CreateTexture("../Assets/textures/sphere/whiteTile/specular.png",   2);
	this->CreateTexture("../Assets/textures/sphere/whiteTile/ao.png",         3);
	this->CreateTexture("../Assets/textures/sphere/noHeight.png",	  4);

	this->CreateTexture("../Assets/textures/sphere/brickCinder/normals.png",  5);
	this->CreateTexture("../Assets/textures/sphere/brickCinder/diffuse.png",  6);
	this->CreateTexture("../Assets/textures/sphere/brickCinder/specular.png", 7);
	this->CreateTexture("../Assets/textures/sphere/brickCinder/ao.png",		  8);
	this->CreateTexture("../Assets/textures/sphere/noHeight.png",      9);

	this->CreateTexture("../Assets/textures/sphere/brickBaked/normals.png",   10);
	this->CreateTexture("../Assets/textures/sphere/brickBaked/diffuse.png",   11);
	this->CreateTexture("../Assets/textures/sphere/brickBaked/specular.png",  12);
	this->CreateTexture("../Assets/textures/sphere/brickBaked/ao.png",        13);
	this->CreateTexture("../Assets/textures/sphere/noHeight.png",	  14);

	this->CreateTexture("../Assets/textures/sphere/blackTile/normals.jpg",    15);
	this->CreateTexture("../Assets/textures/sphere/blackTile/diffuse.jpg",    16);
	this->CreateTexture("../Assets/textures/sphere/blackTile/specular.jpg",   17);
	this->CreateTexture("../Assets/textures/sphere/blackTile/ao.jpg",         18);
	this->CreateTexture("../Assets/textures/sphere/noHeight.png",	  19);

	this->CreateTexture("../Assets/textures/sphere/wood/normals.jpg",         20);
	this->CreateTexture("../Assets/textures/sphere/wood/diffuse.jpg",         21);
	this->CreateTexture("../Assets/textures/sphere/wood/specular.jpg",        22);
	this->CreateTexture("../Assets/textures/sphere/wood/ao.jpg",              23);
	this->CreateTexture("../Assets/textures/sphere/noHeight.png",	  24);

	this->CreateTexture("../Assets/textures/sphere/brickRed/normals.jpg",     25);
	this->CreateTexture("../Assets/textures/sphere/brickRed/diffuse.jpg",     26);
	this->CreateTexture("../Assets/textures/sphere/brickRed/specular.jpg",    27);
	this->CreateTexture("../Assets/textures/sphere/brickRed/ao.jpg",          28);
	this->CreateTexture("../Assets/textures/sphere/brickRed/height.jpg",	  29);
	LUtility::Log("Media Loaded.");

	this->CreateDescriptorPool();		LUtility::Log("Descriptor Pool Created");
	this->CreateDescriptorSet(0);		LUtility::Log("Descriptor Set Created.");
	this->BindGUI();
	this->CreateCommandBuffers();		LUtility::Log("Command Buffers Recorded.");
	this->CreateSemaphores();			LUtility::Log("Semaphores Created.");

	m_ClearR = 0.0f;
	m_ClearG = 0.0f;
	m_ClearB = 0.0f;
}

LVForwardPlusRenderCore::~LVForwardPlusRenderCore() {}


void LVForwardPlusRenderCore::Draw() {
	UpdateUniformBuffer();

	uint32_t imageIndex;
	vkAcquireNextImageKHR(m_VContext->GetDevice(), m_VSwapChain, std::numeric_limits<uint64_t>::max(), m_VImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	VkResult r;
	r = vkWaitForFences(m_VContext->GetDevice(), 1, &m_VRenderFences[imageIndex], VK_TRUE, 100);

	RecordCommandBuffers(imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_VImageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_VCommandBuffers[imageIndex];

	vkResetFences(m_VContext->GetDevice(), 1, &m_VRenderFences[imageIndex]);

	VkSemaphore signalSemaphores[] = { m_VRenderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	if (vkQueueSubmit(m_VContext->GetGraphicsQueue(), 1, &submitInfo, m_VRenderFences[imageIndex]) != VK_SUCCESS) LUtility::RuntimeError("Failed To Submit Draw Command Buffer.");

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_VSwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;
	vkQueuePresentKHR(m_VContext->GetPresentQueue(), &presentInfo);
}

void LVForwardPlusRenderCore::UpdateUniformBuffer() {
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;

	LUtility::UniformBufferObject ubo = {};
	ubo.model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(), time * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), m_VSwapChainExtent.width / (float)m_VSwapChainExtent.height, 0.1f, 10000.0f);
	ubo.proj[1][1] *= -1;
	ubo.time = m_HeightMag;

	void *data;
	vkMapMemory(m_VContext->GetDevice(), m_UniformStagingBufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(m_VContext->GetDevice(), m_UniformStagingBufferMemory);
	LUtility::CopyBuffer(m_UniformStagingBuffer, m_UniformBuffer, sizeof(ubo), m_VContext->GetDevice(), m_VContext->GetCommandPool(), m_VContext->GetGraphicsQueue());

}

void LVForwardPlusRenderCore::Cleanup() {
	vkDeviceWaitIdle(m_VContext->GetDevice());
	ImGui_ImplGlfwVulkan_Shutdown();
}

void LVForwardPlusRenderCore::RegisterCallbackGUI(std::function<void()> _callback) {
	m_GuiCallback = _callback;
}

void LVForwardPlusRenderCore::SetClearColor(float _r, float _g, float _b) {
	m_ClearR = _r; m_ClearG = _g; m_ClearB = _b;
}

void LVForwardPlusRenderCore::UpdateTextureSet(int _set) {
	vkDeviceWaitIdle(m_VContext->GetDevice());
	this->UpdateTextureDescriptorSet(_set);
}

void LVForwardPlusRenderCore::SetHeightMagnitude(float _mag) {
	m_HeightMag = _mag;
}

//Initalization Functionality
//--------------------------------------------------------------------
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

void LVForwardPlusRenderCore::CreateGraphicsPipeline() {
	auto vertShaderCode = LUtility::ReadFile("../Assets/shaders/vert.spv");
	auto fragShaderCode = LUtility::ReadFile("../Assets/shaders/frag.spv");

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

void LVForwardPlusRenderCore::CreateTexture(const std::string& _path, int _i) { 

	LVWrapper<VkImage> stagingImage{ m_VContext->GetDevice(), vkDestroyImage };
	LVWrapper<VkDeviceMemory> stagingImageMemory{ m_VContext->GetDevice(), vkFreeMemory };
	LUtility::CreateTextureImage(_path, stagingImage, stagingImageMemory, m_TextureImages[_i], m_TextureImageMemories[_i], 
								m_VContext->GetDevice(), m_VContext->GetPhysicalDevice(), m_VContext->GetGraphicsQueue(), m_VContext->GetCommandPool() );
	LUtility::CreateTextureImageView(m_TextureImages[_i], m_TextureImageViews[_i], m_VContext->GetDevice());


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

void LVForwardPlusRenderCore::CreateVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

	LVWrapper<VkBuffer>		  stagingBuffer		 { m_VContext->GetDevice(), vkDestroyBuffer };
	LVWrapper<VkDeviceMemory> stagingBufferMemory{ m_VContext->GetDevice(), vkFreeMemory };
	LUtility::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						   stagingBuffer, stagingBufferMemory,
						   m_VContext->GetDevice(), m_VContext->GetPhysicalDevice());

	void *data;
	vkMapMemory(m_VContext->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, m_Vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_VContext->GetDevice(), stagingBufferMemory);

	LUtility::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_VertexBuffer, m_VertexBufferMemory,
		m_VContext->GetDevice(), m_VContext->GetPhysicalDevice());

	LUtility::CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize,
						 m_VContext->GetDevice(), m_VContext->GetCommandPool(), m_VContext->GetGraphicsQueue());
}

void LVForwardPlusRenderCore::CreateIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

	LVWrapper<VkBuffer> stagingBuffer{ m_VContext->GetDevice(), vkDestroyBuffer };
	LVWrapper<VkDeviceMemory> stagingBufferMemory{ m_VContext->GetDevice(), vkFreeMemory };
	LUtility::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							stagingBuffer, stagingBufferMemory,
							m_VContext->GetDevice(), m_VContext->GetPhysicalDevice());
	void *data;
	vkMapMemory(m_VContext->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, m_Indices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_VContext->GetDevice(), stagingBufferMemory);

	LUtility::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_IndexBuffer, m_IndexBufferMemory,
		m_VContext->GetDevice(), m_VContext->GetPhysicalDevice());

	LUtility::CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize,
						m_VContext->GetDevice(), m_VContext->GetCommandPool(), m_VContext->GetGraphicsQueue());
}

void LVForwardPlusRenderCore::CreateDescriptorPool() {
	std::array<VkDescriptorPoolSize, 3> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 5;
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; //Note: Need Extra Combined Image Sampler for IMGUI
	poolSizes[2].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 2;
	if (vkCreateDescriptorPool(m_VContext->GetDevice(), &poolInfo, nullptr, m_VDecriptorPool.Replace()) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Create Descriptor Pool.");
}

void LVForwardPlusRenderCore::CreateDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding sampler0 = {};
	sampler0.binding = 1;
	sampler0.descriptorCount = 1;           
	sampler0.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler0.pImmutableSamplers = nullptr;
	sampler0.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding sampler1 = {};
	sampler1.binding = 2;
	sampler1.descriptorCount = 1;           
	sampler1.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler1.pImmutableSamplers = nullptr;
	sampler1.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding sampler2 = {};
	sampler2.binding = 3;
	sampler2.descriptorCount = 1;
	sampler2.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler2.pImmutableSamplers = nullptr;
	sampler2.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding sampler3 = {};
	sampler3.binding = 4;
	sampler3.descriptorCount = 1;
	sampler3.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler3.pImmutableSamplers = nullptr;
	sampler3.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding sampler4 = {};
	sampler4.binding = 5;
	sampler4.descriptorCount = 1;
	sampler4.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler4.pImmutableSamplers = nullptr;
	sampler4.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	std::array<VkDescriptorSetLayoutBinding, 6> bindings = { uboLayoutBinding, sampler0, sampler1, sampler2, sampler3, sampler4 };
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
	if (vkCreateDescriptorSetLayout(m_VContext->GetDevice(), &layoutInfo, nullptr, m_VDescriptorSetLayout.Replace()) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Create Descriptor Set Layout.");
}

void LVForwardPlusRenderCore::CreateDescriptorSet(int textureSet) {
	VkDescriptorSetLayout layouts[] = { m_VDescriptorSetLayout };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_VDecriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;
	if (vkAllocateDescriptorSets(m_VContext->GetDevice(), &allocInfo, &m_VDescriptorSet) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Allocate Descriptor Set.");

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = m_UniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(LUtility::UniformBufferObject);

	std::array<VkWriteDescriptorSet, 6> descriptorWrites = {};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = m_VDescriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	//Texture 0
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = m_TextureImageViews[0 + (5 * textureSet)];
	imageInfo.sampler = m_VTextureSampler;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = m_VDescriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	//Texture 1
	VkDescriptorImageInfo image1Info = {};
	image1Info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image1Info.imageView = m_TextureImageViews[1 + (5 * textureSet)];
	image1Info.sampler   = m_VTextureSampler;
	
	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[2].dstSet = m_VDescriptorSet;
	descriptorWrites[2].dstBinding = 2;
	descriptorWrites[2].dstArrayElement = 0;
	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[2].descriptorCount = 1;
	descriptorWrites[2].pImageInfo = &image1Info;

	//Texture 2
	VkDescriptorImageInfo image2Info = {};
	image2Info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image2Info.imageView = m_TextureImageViews[2 + (5 * textureSet)];
	image2Info.sampler = m_VTextureSampler;

	descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[3].dstSet = m_VDescriptorSet;
	descriptorWrites[3].dstBinding = 3;
	descriptorWrites[3].dstArrayElement = 0;
	descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[3].descriptorCount = 1;
	descriptorWrites[3].pImageInfo = &image2Info;

	//Texture 3
	VkDescriptorImageInfo image3Info = {};
	image3Info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image3Info.imageView = m_TextureImageViews[3 + (5 * textureSet)]; 
	image3Info.sampler = m_VTextureSampler;

	descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[4].dstSet = m_VDescriptorSet;
	descriptorWrites[4].dstBinding = 4;
	descriptorWrites[4].dstArrayElement = 0;
	descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[4].descriptorCount = 1;
	descriptorWrites[4].pImageInfo = &image3Info;

	//Texture 4
	VkDescriptorImageInfo image4Info = {};
	image4Info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image4Info.imageView = m_TextureImageViews[4 + (5 * textureSet)];
	image4Info.sampler = m_VTextureSampler;

	descriptorWrites[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[5].dstSet = m_VDescriptorSet;
	descriptorWrites[5].dstBinding = 5;
	descriptorWrites[5].dstArrayElement = 0;
	descriptorWrites[5].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[5].descriptorCount = 1;
	descriptorWrites[5].pImageInfo = &image4Info;
	
	vkUpdateDescriptorSets(m_VContext->GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void LVForwardPlusRenderCore::UpdateTextureDescriptorSet(int _set) {

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = m_UniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(LUtility::UniformBufferObject);

	std::array<VkWriteDescriptorSet, 6> descriptorWrites = {};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = m_VDescriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	//Texture 0
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = m_TextureImageViews[0 + (5 * _set)];
	imageInfo.sampler = m_VTextureSampler;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = m_VDescriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	//Texture 1
	VkDescriptorImageInfo image1Info = {};
	image1Info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image1Info.imageView = m_TextureImageViews[1 + (5 * _set)];
	image1Info.sampler = m_VTextureSampler;

	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[2].dstSet = m_VDescriptorSet;
	descriptorWrites[2].dstBinding = 2;
	descriptorWrites[2].dstArrayElement = 0;
	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[2].descriptorCount = 1;
	descriptorWrites[2].pImageInfo = &image1Info;

	//Texture 2
	VkDescriptorImageInfo image2Info = {};
	image2Info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image2Info.imageView = m_TextureImageViews[2 + (5 * _set)];
	image2Info.sampler = m_VTextureSampler;

	descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[3].dstSet = m_VDescriptorSet;
	descriptorWrites[3].dstBinding = 3;
	descriptorWrites[3].dstArrayElement = 0;
	descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[3].descriptorCount = 1;
	descriptorWrites[3].pImageInfo = &image2Info;

	//Texture 3
	VkDescriptorImageInfo image3Info = {};
	image3Info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image3Info.imageView = m_TextureImageViews[3 + (5 * _set)];
	image3Info.sampler = m_VTextureSampler;

	descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[4].dstSet = m_VDescriptorSet;
	descriptorWrites[4].dstBinding = 4;
	descriptorWrites[4].dstArrayElement = 0;
	descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[4].descriptorCount = 1;
	descriptorWrites[4].pImageInfo = &image3Info;

	//Texture 4
	VkDescriptorImageInfo image4Info = {};
	image4Info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image4Info.imageView = m_TextureImageViews[4 + (5 * _set)];
	image4Info.sampler = m_VTextureSampler;

	descriptorWrites[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[5].dstSet = m_VDescriptorSet;
	descriptorWrites[5].dstBinding = 5;
	descriptorWrites[5].dstArrayElement = 0;
	descriptorWrites[5].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[5].descriptorCount = 1;
	descriptorWrites[5].pImageInfo = &image4Info;

	vkUpdateDescriptorSets(m_VContext->GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}


void LVForwardPlusRenderCore::CreateCommandBuffers() {

	m_VRenderCommandPools.resize(m_VSwapChainFramebuffers.size(), LVWrapper<VkCommandPool>{ m_VContext->GetDevice(), vkDestroyCommandPool });
	m_VCommandBuffers.resize(m_VSwapChainFramebuffers.size());
	m_VRenderFences.resize(m_VSwapChainFramebuffers.size(), LVWrapper<VkFence>{ m_VContext->GetDevice(), vkDestroyFence });
	for (int i = 0; i < m_VSwapChainFramebuffers.size(); ++i) {

		//Command Pools
		LUtility::QueueFamilyIndices indices = LUtility::FindQueueIndices(m_VContext->GetPhysicalDevice(), m_VContext->GetSurface());

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = indices.graphicsFamily;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(m_VContext->GetDevice(), &poolInfo, nullptr, m_VRenderCommandPools[i].Replace()) != VK_SUCCESS)
			LUtility::RuntimeError("Failed To Create Command Pool.");

		//Command Buffers
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_VRenderCommandPools[i];
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;
		if (vkAllocateCommandBuffers(m_VContext->GetDevice(), &allocInfo, &m_VCommandBuffers[i]) != VK_SUCCESS)
			LUtility::RuntimeError("Failed To Allocate Command Buffers.");

		//Fences
		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		if (vkCreateFence(m_VContext->GetDevice(), &fenceInfo, nullptr, m_VRenderFences[i].Replace()))
			LUtility::RuntimeError("Failed To Create Fence.");
	}
}

void LVForwardPlusRenderCore::CreateSemaphores() {
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(m_VContext->GetDevice(), &semaphoreInfo, nullptr, m_VImageAvailableSemaphore.Replace()) != VK_SUCCESS ||
		vkCreateSemaphore(m_VContext->GetDevice(), &semaphoreInfo, nullptr, m_VRenderFinishedSemaphore.Replace()) != VK_SUCCESS)
		LUtility::RuntimeError("Failed To Create Semaphores.");
}

void LVForwardPlusRenderCore::RecordCommandBuffers(int i) {

	vkResetCommandPool(m_VContext->GetDevice(), m_VRenderCommandPools[i], 0);

	//Record Draw Command
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;
	vkBeginCommandBuffer(m_VCommandBuffers[i], &beginInfo);

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_VRenderPass;
	renderPassInfo.framebuffer = m_VSwapChainFramebuffers[i];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_VSwapChainExtent;

	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0].color = { m_ClearR, m_ClearG, m_ClearB, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();
	vkCmdBeginRenderPass(m_VCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	//GUI
	//------------------------------------
	ImGui_ImplGlfwVulkan_NewFrame();
	m_GuiCallback();
	//------------------------------------


	vkCmdBindPipeline(m_VCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_VGraphicsPipeline);
	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(m_VCommandBuffers[i], 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(m_VCommandBuffers[i], m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(m_VCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_VGraphicsPipelineLayout, 0, 1, &m_VDescriptorSet, 0, nullptr);
	vkCmdDrawIndexed(m_VCommandBuffers[i], static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);

	//GUI
	//-------------------------------------
	ImGui_ImplGlfwVulkan_Render(m_VCommandBuffers[i]);
	//-------------------------------------

	vkCmdEndRenderPass(m_VCommandBuffers[i]);
	if (vkEndCommandBuffer(m_VCommandBuffers[i]) != VK_SUCCESS)	LUtility::RuntimeError("Failed To Record Command Buffer.");
}

void LVForwardPlusRenderCore::BindGUI() {
	ImGui_ImplGlfwVulkan_Init_Data init = {};
	init.allocator = nullptr;
	init.gpu = m_VContext->GetPhysicalDevice();
	init.device = m_VContext->GetDevice();
	init.render_pass = m_VRenderPass;
	init.pipeline_cache = VK_NULL_HANDLE;
	init.descriptor_pool = m_VDecriptorPool;
	init.check_vk_result = nullptr;
	ImGui_ImplGlfwVulkan_Init(m_VContext->GetWindowHandle(), true, &init);

	VkCommandBuffer commandBuffer = LUtility::BeginSingleTimeCommands(m_VContext->GetCommandPool(), m_VContext->GetDevice());
	ImGui_ImplGlfwVulkan_CreateFontsTexture(commandBuffer);
	LUtility::EndSingleTimeCommands(commandBuffer, m_VContext->GetGraphicsQueue(), m_VContext->GetCommandPool(), m_VContext->GetDevice());
	ImGui_ImplGlfwVulkan_InvalidateFontUploadObjects();
}