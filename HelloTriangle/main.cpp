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
#include <vector>

class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	VkInstance instance;
	GLFWwindow * window;
	const int WIDTH = 800;
	const int HEIGHT = 600;

	const std::vector<const char *> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;

#else
	const bool enableValidationLayers = true;
#endif // NDEBUG



	void initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(WIDTH, HEIGHT, "Hello Triangle", nullptr, nullptr);
	}

	void createInstance() {
		if (enableValidationLayers && !allValidationLayersArePresent()) {
			throw std::runtime_error("Missing support for requested validation Layers.");
		}

		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = "Hello Triangle";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = "No Engine";
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &applicationInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}
		

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}

		VerifyGLFWRequiredExtensionsPresent(glfwExtensions, glfwExtensionCount);
	}

	bool VerifyGLFWRequiredExtensionsPresent(const char* const* glfwRequiredExtensions, uint32_t glfwExtensionCount)
	{	
		bool result = 1;
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(13);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		bool extensionFound = true;
		for(auto i = 0; i < glfwExtensionCount; i++)
		{
			extensionFound = false;
			for (const auto& extension : extensions)
			{
				std::string supportedExtensionName(extension.extensionName);
				std::string requiredExtensionName(glfwRequiredExtensions[i]);
				if (supportedExtensionName == requiredExtensionName) {
					extensionFound = true;
					continue;
				}
			}
			if (extensionFound == false)
			{
				result = 0;
				break;
			}
		};
		return result;
	}

	bool allValidationLayersArePresent() {
		bool result = true;
		uint32_t supportedLayerCount;
		vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(supportedLayerCount);
		vkEnumerateInstanceLayerProperties(&supportedLayerCount, availableLayers.data());
		for (const char * layerName : validationLayers) {
			bool layerIsPresent = false;
			for (const auto& availableLayer : availableLayers) {
				if (strcmp(availableLayer.layerName, layerName) == 0) {
					layerIsPresent = true;
				}
			}
			if (!layerIsPresent) {
				result = false;
			}
		}
		return result;
	}

	void initVulkan() {
		createInstance();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	void cleanup() {
		vkDestroyInstance(instance, nullptr);

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