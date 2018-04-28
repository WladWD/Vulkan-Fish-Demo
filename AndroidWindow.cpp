#include "AndroidWindow.h"

VulkanEngineApplication::AndroidWindow::AndroidWindow(int32_t width, int32_t height, const char * sTitle) {
    InitVulkan();

    window = new VulkanWindow();
    vulkanData = const_cast<VulkanData *>(window->getVulkanData());
    vulkanData->mSwapChainImageExtent.height = height;
    vulkanData->mSwapChainImageExtent.width = width;
    addInstanceExtension();
    window->createInstance();
}

VulkanEngineApplication::AndroidWindow::~AndroidWindow() {
    vkDestroySurfaceKHR(vulkanData->vulkanInstance, vulkanData->windowSurface, nullptr);
}

void VulkanEngineApplication::AndroidWindow::createWindowSurface(ANativeWindow* window) {
	VkAndroidSurfaceCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.window = window;

	if (vkCreateAndroidSurfaceKHR(vulkanData->vulkanInstance, &createInfo, nullptr, &vulkanData->windowSurface) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] failed to create surface");
	}

    this->window->initialize();
}

void VulkanEngineApplication::AndroidWindow::addInstanceExtension(void) {
    window->addInstanceRequiredExtension("VK_KHR_surface");//VK_KHR_android_surface
    window->addInstanceRequiredExtension("VK_KHR_android_surface");//VK_KHR_android_surface
}

void VulkanEngineApplication::AndroidWindow::windowResize(ANativeWindow* window, int32_t width, int32_t height)
{
    vulkanData->mSwapChainImageExtent.height = height;
    vulkanData->mSwapChainImageExtent.width = width;

    vkDestroySurfaceKHR(vulkanData->vulkanInstance, vulkanData->windowSurface, nullptr);
    this->createWindowSurface(window);
    this->window->resize();
}

void VulkanEngineApplication::AndroidWindow::pause(void)
{
    window->pause();
}

void VulkanEngineApplication::AndroidWindow::resume(void)
{
    window->resume();
}

void VulkanEngineApplication::AndroidWindow::draw(void)
{
    window->draw();
}
