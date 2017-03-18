#pragma once

#include <memory>

class LWindow;
class LVForwardPlusRenderCore;

class LApplication {
public:
	LApplication();
	~LApplication();

	void Initialize(int argc, const char *argv[]);
	void Run();
	void OnGUI();
	void Update(float _dt);

private:
	std::unique_ptr<LWindow>				 m_Window;
	std::unique_ptr<LVForwardPlusRenderCore> m_Renderer;
	float m_TestFloat;
};