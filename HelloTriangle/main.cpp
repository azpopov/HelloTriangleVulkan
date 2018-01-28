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

VkResult CreateDebugReportCallbackEXT(VkInstance instance,
	const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks * pAllocator, VkDebugReportCallbackEXT* pCallback)
{
	auto func = PFN_vkCreateDebugReportCallbackEXT(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
	if(func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugReportCallbackEXT(VkInstance instance,
	VkDebugReportCallbackEXT callback,
	const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if(func != nullptr)
	{
		func(instance, callback, pAllocator);
	}
}

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
	VkDevice device;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDebugReportCallbackEXT callback;
	GLFWwindow * window;
	VkQueue graphicsQueue;
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

	struct QueueFamiliesIndices
	{
		int graphicsFamily = -1;

		bool isComplete()
		{
			return graphicsFamily >= 0;
		}
	};

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

		auto extensions = get_required_extensions();

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

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
		
	}

	QueueFamiliesIndices findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamiliesIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for(const auto& queueFamily : queueFamilies)
		{
			if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}
			if(indices.isComplete())
			{
				break;
			}
			i++;
		}
		return indices;
	}

	std::vector<const char *> get_required_extensions()
	{
		uint32_t extensionCount = 0;
		const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
		std::vector<const char *> extensions(glfwExtensions, glfwExtensions + extensionCount);
		if(enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
		return extensions;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char * layerPrefix,
		const char * msg,
		void* userData
		)
	{
		std::cerr << "validation layers: " << msg << std::endl;
		return VK_FALSE;
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

	void setupDebugCallback()
	{
		if (!enableValidationLayers) return;
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = debugCallback;

		if (CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback)
			!= VK_SUCCESS)
		{
			throw std::runtime_error("failed to setup debug callback");
		}
	}

	void pickPhysicalDevice()
	{
		
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if(deviceCount == 0)
		{
			throw std::runtime_error("failed to find GPUs with Vulkan support");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if(isDeviecSuitable(device))
			{
				physicalDevice = device;
				break;
			}
		}

		if(physicalDevice == VK_NULL_HANDLE)
		{
			throw std::runtime_error("failed to find suitable GPU!");
		}
	}

	bool isDeviecSuitable(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		QueueFamiliesIndices indices = findQueueFamilies(device);

		const auto isIndiceComplete = indices.isComplete();
		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 
			&& deviceFeatures.geometryShader && isIndiceComplete;
	}

	void createLogicalDevice()
	{
		QueueFamiliesIndices indices = findQueueFamilies(physicalDevice);

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;
		if(enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}
		if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}
		vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
	}

	void initVulkan() {
		createInstance();
		setupDebugCallback();
		pickPhysicalDevice();
		createLogicalDevice();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	void cleanup() {
		DestroyDebugReportCallbackEXT(instance, callback, nullptr);

		vkDestroyInstance(instance, nullptr);
		vkDestroyDevice(device, nullptr);
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