#pragma once

#include <memory>

class LVContext;
struct GLFWwindow;

class LVForwardPlusRenderCore {
public:
	LVForwardPlusRenderCore(GLFWwindow *_window);
	~LVForwardPlusRenderCore();

private:
	std::unique_ptr<LVContext> m_VContext;
};