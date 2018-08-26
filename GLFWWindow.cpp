#include "GLFWWindow.h"


VulkanEngineApplication::GLFWWindow::GLFWWindow(int32_t width, int32_t height, const char * sTitle) : vulkanData(nullptr), glfwWindow(nullptr), window(nullptr) {
	window = new VulkanWindow();
	assetLoader = new Asset::FileAssetLoader();
	vulkanData = window->getVulkanData();
	initWindow(width, height, sTitle);

	addInstanceExtension();
	window->createInstance();
	
	createWindowSurface();
	window->initialize(assetLoader);
}

VulkanEngineApplication::GLFWWindow::~GLFWWindow() {
	delete window;
	delete assetLoader;

	glfwDestroyWindow(glfwWindow);
	glfwTerminate();
}

void VulkanEngineApplication::GLFWWindow::initWindow(int32_t width, int32_t height, const char * sTitle) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	glfwWindow = glfwCreateWindow(width, height, sTitle, nullptr, nullptr);

	auto data = const_cast<VulkanData *>(vulkanData);
	data->mSwapChainImageExtent.height = height;
	data->mSwapChainImageExtent.width = width;

	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetWindowSizeCallback(glfwWindow, onWindowResize);
}

void VulkanEngineApplication::GLFWWindow::createWindowSurface(void) {

	auto data = const_cast<VulkanData *>(vulkanData);
	VkResult result;
	if ((result = glfwCreateWindowSurface(vulkanData->vulkanInstance, glfwWindow, nullptr, &data->windowSurface)) != VK_SUCCESS) {
		throw std::runtime_error("[Error] Failed to create window surface!");
	}
}

void VulkanEngineApplication::GLFWWindow::addInstanceExtension(void) {
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtension;
	glfwExtension = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		window->addInstanceRequiredExtension(glfwExtension[i]);
}

void VulkanEngineApplication::GLFWWindow::onWindowResize(GLFWwindow * mWindow, int width, int height) {
	if (!width || !height)
		return;

	VulkanEngineApplication::GLFWWindow *mApplication = static_cast<VulkanEngineApplication::GLFWWindow *>(glfwGetWindowUserPointer(mWindow));

	auto data = const_cast<VulkanData *>(mApplication->vulkanData);
	data->mSwapChainImageExtent.height = height;
	data->mSwapChainImageExtent.width = width;

	mApplication->window->resize();
}

void VulkanEngineApplication::GLFWWindow::run(void) {
	while (!glfwWindowShouldClose(glfwWindow)) {
		glfwPollEvents();
		window->draw();
	}
}

