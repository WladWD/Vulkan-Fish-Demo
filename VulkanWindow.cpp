#include "VulkanWindow.h"

VulkanEngineApplication::VulkanWindow::VulkanWindow(void) {
	device = new VulkanDevice(&vulkanData);
	swapchain = new VulkanSwapchain(&vulkanData);
	framebuffer = new VulkanFramebuffer(&vulkanData);
	command = new VulkanCommand(&vulkanData);
	debug = new VulkanDebug(&vulkanData);
	info = new VulkanInfo(&vulkanData);
	engine = new Engine::VulkanEngine(&vulkanData);
}

VulkanEngineApplication::VulkanWindow::~VulkanWindow() {
	
	vkDeviceWaitIdle(vulkanData.device);

	delete engine;

	delete info;

	vkDestroySemaphore(vulkanData.device, vulkanData.imageAvailable, nullptr);

	vkDestroySemaphore(vulkanData.device, vulkanData.renderFinish, nullptr);

	delete debug;

	delete command;

	delete framebuffer;

	delete swapchain;

	delete device;

	vkDestroyInstance(vulkanData.vulkanInstance, nullptr);
}

const VulkanEngineApplication::VulkanData * VulkanEngineApplication::VulkanWindow::getVulkanData() {
	return &vulkanData;
}

void VulkanEngineApplication::VulkanWindow::initialize(void) {
	initVulkan();
	engine->initialize();

#ifdef _DEBUG
	info->printApplicationInfo();
#endif
}

void VulkanEngineApplication::VulkanWindow::resize(void) {
	vkDeviceWaitIdle(vulkanData.device);

	swapchain->resize();

	framebuffer->free();
	framebuffer->initialize();

	command->free();
	command->initialize();

	engine->resize();
}

void VulkanEngineApplication::VulkanWindow::pause(void) {
	//stop timer and other object

	engine->pause();
}

void VulkanEngineApplication::VulkanWindow::resume(void) {
	//resume timer and other object
	engine->resume();
}

void VulkanEngineApplication::VulkanWindow::draw() {

	vkQueueWaitIdle(vulkanData.queue[0]);

	VkResult mResult = vkAcquireNextImageKHR(vulkanData.device, vulkanData.swapchain, std::numeric_limits<uint64_t>::max(), vulkanData.imageAvailable, VK_NULL_HANDLE, &vulkanData.mImageIndex);

	if (mResult == VK_ERROR_OUT_OF_DATE_KHR) {
		resize();
		return;
	}
	else if (mResult != VK_SUCCESS && mResult != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("[DBG]\tFailed to aquire swap chain images!");
	}
	///////////////////////////////////////////////////////////////////////////////////////
	vkResetCommandBuffer(vulkanData.commandBuffer[vulkanData.mImageIndex], 0);
	///////////////////////////////////////////////////////////////////////////////////////
	// Drawing
	engine->draw();
	///////////////////////////////////////////////////////////////////////////////////////

	VkSubmitInfo mSubmitInfo = {};
	mSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore mWaitSemaphore[] = { vulkanData.imageAvailable };
	VkPipelineStageFlags mWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	mSubmitInfo.waitSemaphoreCount = 1;
	mSubmitInfo.pWaitSemaphores = mWaitSemaphore;
	mSubmitInfo.pWaitDstStageMask = mWaitStages;
	mSubmitInfo.commandBufferCount = 1;
	mSubmitInfo.pCommandBuffers = &vulkanData.commandBuffer[vulkanData.mImageIndex];

	VkSemaphore mSigmalSemaphores[] = { vulkanData.renderFinish };
	mSubmitInfo.pSignalSemaphores = mSigmalSemaphores;
	mSubmitInfo.signalSemaphoreCount = 1;

	VkSwapchainKHR mSwapChains[] = { vulkanData.swapchain };

	VkPresentInfoKHR mPresentInfo = {};
	mPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	mPresentInfo.waitSemaphoreCount = 1;
	mPresentInfo.pWaitSemaphores = mSigmalSemaphores;
	mPresentInfo.swapchainCount = 1;
	mPresentInfo.pSwapchains = mSwapChains;
	mPresentInfo.pImageIndices = &vulkanData.mImageIndex;
	mPresentInfo.pResults = nullptr;

	VkResult error = vkQueueSubmit(vulkanData.queue[0], 1, &mSubmitInfo, VK_NULL_HANDLE);
	if (error != VK_SUCCESS) {
		throw std::runtime_error("[DGB]\tFailed to submit draw command to buffer!");
	}

	vkQueuePresentKHR(vulkanData.queue[0], &mPresentInfo);
}

void VulkanEngineApplication::VulkanWindow::initVulkan(void) {
	
	initDebug();

	device->initialize();
	swapchain->initialize();
	framebuffer->initialize();
	command->initialize();

	createSemaphore();
}

void VulkanEngineApplication::VulkanWindow::initDebug(void) {
	if (!enableInstanceValidationLayer)
		return;

	debug->initialize();
}

void VulkanEngineApplication::VulkanWindow::createInstance(void) {

	VkApplicationInfo appInfo = { };
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.pApplicationName = "VulkanDemo";
	appInfo.pEngineName = "RGBEngine";
	appInfo.pNext = nullptr;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	VkInstanceCreateInfo instanceInfo = { };
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	
	if (checkValidationSupport() && enableInstanceValidationLayer) {
		instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceValidationLayer.size());
		instanceInfo.ppEnabledLayerNames = instanceValidationLayer.data();
	}
	else {
		instanceInfo.enabledLayerCount = 0;
		instanceInfo.ppEnabledLayerNames = nullptr;
	}

	if (enableInstanceValidationLayer) {
		instanceExtension.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtension.size());
	instanceInfo.ppEnabledExtensionNames = instanceExtension.data();

	if (vkCreateInstance(&instanceInfo, nullptr, &vulkanData.vulkanInstance) != VK_SUCCESS) {
		throw std::runtime_error("[Error] Failed create instace!");
	}
}

void VulkanEngineApplication::VulkanWindow::addInstanceRequiredExtension(const char *extensionName) {
	instanceExtension.push_back(extensionName);
}

bool VulkanEngineApplication::VulkanWindow::checkValidationSupport(void) {

	std::vector<VkLayerProperties> properties;
	uint32_t propertiesCount = 0;
	vkEnumerateInstanceLayerProperties(&propertiesCount, nullptr);
	if (propertiesCount > 0) {
		properties.resize(propertiesCount);
		vkEnumerateInstanceLayerProperties(&propertiesCount, properties.data());

		int32_t findCount = 0;
		for (auto ls : instanceValidationLayer) {
			for (auto al : properties) {
				findCount += !strcmp(ls, al.layerName);
			}
		}
		return findCount == instanceValidationLayer.size();
	}

	return false;
}

void VulkanEngineApplication::VulkanWindow::createSemaphore(void) {
	VkSemaphoreCreateInfo mSemaphoreCreateInfo = {};
	mSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(vulkanData.device, &mSemaphoreCreateInfo, nullptr, &vulkanData.imageAvailable) != VK_SUCCESS ||
		vkCreateSemaphore(vulkanData.device, &mSemaphoreCreateInfo, nullptr, &vulkanData.renderFinish) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to create semaphores!");
	}
}
