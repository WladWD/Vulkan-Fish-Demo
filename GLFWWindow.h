#include "VulkanWindow.h"
#include <iostream>
#include "FileAssetLoader.h"
#pragma once

namespace VulkanEngineApplication {
	class GLFWWindow {
		GLFWwindow *glfwWindow;
		const VulkanData *vulkanData;

		VulkanWindow *window;
		Asset::AssetLoader *assetLoader;
		
		void initWindow(int32_t width, int32_t height, const char *sTitle);
		void createWindowSurface(void);
		void addInstanceExtension(void);
		static void onWindowResize(GLFWwindow * mWindow, int width, int height);
	public:
		GLFWWindow(int32_t width, int32_t height, const char *sTitle = "Vulkan");
		~GLFWWindow();

		void run(void);
	};
}

