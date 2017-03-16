#pragma once

#include <GLFW\glfw3.h>

#include <memory>

class LWindow;
class LVForwardPlusRenderCore;

class LApplication {
public:
	LApplication();
	~LApplication();

	void Initialize(int argc, const char *argv[]);
	void Run();
	void Update();

private:
	std::unique_ptr<LWindow>				 m_Window;
	std::unique_ptr<LVForwardPlusRenderCore> m_Renderer;
};