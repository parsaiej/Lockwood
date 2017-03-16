#include "..\LVForwardPlusRenderCore.h"
#include "..\LVContext.h"

LVForwardPlusRenderCore::LVForwardPlusRenderCore(GLFWwindow *_window) 
	: m_VContext(std::make_unique<LVContext>(_window))
{
	printf("Vulkan Initialized.\n");
}

LVForwardPlusRenderCore::~LVForwardPlusRenderCore() {}