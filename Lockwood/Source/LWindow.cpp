#include <iostream>
#include <GLFW\glfw3.h>

#include "..\LWindow.h"
#include "..\LUtility.h"

LWindow::LWindow(unsigned int _width, unsigned int _height, const char *_title) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_glfwWindow = glfwCreateWindow(_width, _height, _title, nullptr, nullptr);
	glfwSetWindowUserPointer(m_glfwWindow, this);

	if (!m_glfwWindow) LUtility::RuntimeError("Error Initializing Window.");
	LUtility::Log("Window Created.");
}

LWindow::~LWindow() {

}

int LWindow::ShouldClose() {
	return glfwWindowShouldClose(m_glfwWindow);
}

void LWindow::PollEvents() {
	glfwPollEvents();
}

void LWindow::Close() {
	glfwDestroyWindow(m_glfwWindow);
	glfwTerminate();
}