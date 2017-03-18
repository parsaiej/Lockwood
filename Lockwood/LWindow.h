#pragma once

struct GLFWwindow;

class LWindow {
public:
	LWindow(unsigned int _width, unsigned int _height, const char *_title);
	~LWindow();

	int ShouldClose();
	void PollEvents();
	void Close();

	void RegisterCallbackCursorPosition() {};
	void RegisterCallbackMouseButton() {};
	void RegisterCallbackKeyboard() {};

	inline GLFWwindow* GetHandle() { return m_glfwWindow; };

private:
	GLFWwindow *m_glfwWindow;
};