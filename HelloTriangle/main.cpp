#define GLFW_INCLUDE_VULKAN
#include "..\..\glfw-3.2.1.bin.WIN64\include\GLFW\glfw3.h"
#include "..\..\glfw-3.2.1.bin.WIN64\include\GLFW\glfw3native.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "..\..\glm-0.9.9-a2\glm\vec3.hpp"
#include "..\..\glm-0.9.9-a2\glm\mat4x4.hpp"

#include <stdexcept>
#include <functional>
#include <iostream>

class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	GLFWwindow * window;
	const int WIDTH = 800;
	const int HEIGHT = 600;
	void initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(WIDTH, HEIGHT, "Hello Triangle", nullptr, nullptr);
	}

	void initVulkan() {

	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	void cleanup() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}