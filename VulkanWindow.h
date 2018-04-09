#include "VulkanEngine.h"
#include <iostream>
#pragma once

namespace VulkanEngineApplication 
{
	class VulkanWindow 
	{
		/////////////////////////////////////////////////////////////////////////////////////
		VkInstance vulkanInstance;
		/////////////////////////////////////////////////////////////////////////////////////
		//Validation Layer
		std::vector<const char *> deviceValidationLayer;
		std::vector<const char *> instanceValidationLayer = {
			"VK_LAYER_LUNARG_standard_validation"
		};

#ifdef _DEBUG
		const bool enableDeviceValidationLayer = true;
		const bool enableInstanceValidationLayer = true;
#else
		const bool enableDeviceValidationLayer = true;
		const bool enableInstanceValidationLayer = false;
#endif

		VkDebugReportCallbackEXT mCallbackDebugReport;
		/////////////////////////////////////////////////////////////////////////////////////
		//Extension 
		std::vector<const char *> deviceExtension;
		std::vector<const char *> instanceExtension;
		/////////////////////////////////////////////////////////////////////////////////////
		//GLFW DATA
		GLFWwindow *mWindow;
		/////////////////////////////////////////////////////////////////////////////////////
		//Android DATA

		/////////////////////////////////////////////////////////////////////////////////////
		//Vulkan DATA
		VkSurfaceKHR windowSurface;
		/////////////////////////////////////////////////////////////////////////////////////
		static void OnWindowResize(GLFWwindow *mWindow, int width, int height);
		/////////////////////////////////////////////////////////////////////////////////////
		//Print Function
		void printApplicationInfo(void);

		void printInstanceExtensionInfo(void);
		void printDeviceExtensionInfo(void);

		void printInstanceValidationLayerInfo(void);
		void printDeviceValidationLayerInfo(void);
		/////////////////////////////////////////////////////////////////////////////////////
		//Check Function
		bool checkValidationSupport(void);
		void addInstanceRequiredExtension(void);
		void selectPhisicalDevice(void);
		/////////////////////////////////////////////////////////////////////////////////////
		void initWindow(int32_t width, int32_t height, const char *sTitle = "Vulkan");
		void initVulkan(void);
		/////////////////////////////////////////////////////////////////////////////////////
		//Debug Report Function
		VkResult createDebugReportCallbackEXT(VkInstance mxInstance,
			const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
			const VkAllocationCallbacks *mAllocators,
			VkDebugReportCallbackEXT *pCallback);
		void destroyDebugReportCallbackEXT(VkInstance mxInstance,
			VkDebugReportCallbackEXT pCallback,
			const VkAllocationCallbacks *mAllocators);
		void setupDebugReport();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objType,
			uint64_t obj,
			size_t location,
			int32_t code,
			const char *layerPrefix,
			const char *msg,
			void *userData
		);
		/////////////////////////////////////////////////////////////////////////////////////
		void createInstance(void);
		void createWindowSurface(void);
		void createDevice(void);
		void createSwapShain(void);
		/////////////////////////////////////////////////////////////////////////////////////
	public:
		VulkanWindow(int32_t width, int32_t height, const char *sTitle = "Vulkan");
		~VulkanWindow();

		void run();
	};
}

