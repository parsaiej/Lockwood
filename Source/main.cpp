#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include "GLFW\glfw3.h"

#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

class Lockwood {
public:
	void Run() {
		InitializeWindow();
		CoreLoop();
	}

private:

	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;
	GLFWwindow *m_Window;

	void InitializeWindow() {
		
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		m_Window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lockwood", nullptr, nullptr);

	}

	void CoreLoop() {
		while (!glfwWindowShouldClose(m_Window)) {
			glfwPollEvents();
		}

		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

};

int main(){
	
	Lockwood engine;

	try {
		engine.Run();
	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}