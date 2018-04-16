#include "VulkanWindow.h"

VulkanEngineApplication::VulkanWindow::VulkanWindow(void) {
	engine = new Engine::VulkanEngine(&vulkanData);
}

VulkanEngineApplication::VulkanWindow::~VulkanWindow() {
	
	delete engine;

	vkDestroySurfaceKHR(vulkanData.vulkanInstance, vulkanData.windowSurface, nullptr);

	destroyDebugReportCallbackEXT(vulkanData.vulkanInstance, mCallbackDebugReport, nullptr);

	vkDestroyInstance(vulkanData.vulkanInstance, nullptr);
}

const VulkanEngineApplication::VulkanData * VulkanEngineApplication::VulkanWindow::getVulkanData() {
	return &vulkanData;
}

void VulkanEngineApplication::VulkanWindow::initialize(void) {
	initVulkan();
	engine->initialize();

#ifdef _DEBUG
	printApplicationInfo();
#endif
}

void VulkanEngineApplication::VulkanWindow::resize(void) {
	//resize swap chain

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
	
	setupDebugReport();
	createDevice();
	createSwapchain();
	createSwapchainImageView();
	createRenderPass();
	createFramebuffer();
	createCommandPool();
	createCommandBuffer();
	createSemaphore();
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

void VulkanEngineApplication::VulkanWindow::selectPhisicalDevice(void) {
	std::vector<VkPhysicalDevice> phisicalDevices;
	uint32_t deviceCount = 0u;
	vkEnumeratePhysicalDevices(vulkanData.vulkanInstance, &deviceCount, nullptr);
	if (deviceCount > 0) {
		phisicalDevices.resize(deviceCount);
		vkEnumeratePhysicalDevices(vulkanData.vulkanInstance, &deviceCount, phisicalDevices.data());
		for (const auto &device : phisicalDevices) {
			int32_t queueFamilyIndex = -1;
			if (isDeviceSuitable(device, queueFamilyIndex)) {
				vulkanData.physicalDevice = device;
				this->vulkanData.queueFamilyIndex = queueFamilyIndex;
				break;
			}
		}
	}
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

int32_t VulkanEngineApplication::VulkanWindow::findQueueFamily(VkPhysicalDevice mDevice)
{
	uint32_t mQueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(mDevice, &mQueueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> mQueueFamilyProperties(mQueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mDevice, &mQueueFamilyCount, mQueueFamilyProperties.data());

	int32_t mCurrentQueueFamilyIndex = 0;
	bool mFind = false;
	for (const auto &mQueueFamily : mQueueFamilyProperties) {
		VkBool32 bPhysicalDeviceSurfaceSupport = false;

		vkGetPhysicalDeviceSurfaceSupportKHR(mDevice, mCurrentQueueFamilyIndex, vulkanData.windowSurface, &bPhysicalDeviceSurfaceSupport);

		if (bPhysicalDeviceSurfaceSupport &&
			mQueueFamily.queueCount > 0 &&
			(mQueueFamily.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))) {
			mFind = true;
			break;
		}

		++mCurrentQueueFamilyIndex;
	}

	return mFind ? mCurrentQueueFamilyIndex : (-1);
}

bool VulkanEngineApplication::VulkanWindow::checkDeviceExtensionSupport(VkPhysicalDevice mDevice)
{
	uint32_t mExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(mDevice, nullptr, &mExtensionCount, nullptr);

	std::vector<VkExtensionProperties> mAvailableDeviceExtensions(mExtensionCount);
	vkEnumerateDeviceExtensionProperties(mDevice, nullptr, &mExtensionCount, mAvailableDeviceExtensions.data());

	std::set<std::string> mRequiredExtension(deviceExtension.begin(), deviceExtension.end());
	for (const auto &mAvailableDeviceExtension : mAvailableDeviceExtensions)
		mRequiredExtension.erase(mAvailableDeviceExtension.extensionName);

	return mRequiredExtension.empty();
}

bool VulkanEngineApplication::VulkanWindow::isDeviceSuitable(VkPhysicalDevice mDevice, int32_t &mDeviceQueueFamilyIndex) {
	VkPhysicalDeviceProperties mDeviceProperties;
	VkPhysicalDeviceFeatures mDeviceFeatures;
	vkGetPhysicalDeviceProperties(mDevice, &mDeviceProperties);
	vkGetPhysicalDeviceFeatures(mDevice, &mDeviceFeatures);

	mDeviceQueueFamilyIndex = findQueueFamily(mDevice);

	bool mDeviceAvailableExtensionSupport = checkDeviceExtensionSupport(mDevice);
	bool mSwapChainAdequate = false;
	if (mDeviceAvailableExtensionSupport) {
		auto mSwapChainProperties = querySwapChainSupport(mDevice);
		mSwapChainAdequate = !mSwapChainProperties.mFormats.empty() && !mSwapChainProperties.mFormats.empty();
	}

	return	(mDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) &&
		mDeviceAvailableExtensionSupport &&
		mSwapChainAdequate &&
		mDeviceFeatures.geometryShader &&
		mDeviceFeatures.tessellationShader &&
		mDeviceFeatures.samplerAnisotropy &&
		(mDeviceQueueFamilyIndex >= 0);
}

void VulkanEngineApplication::VulkanWindow::createDevice(void) {
	selectPhisicalDevice();

	const float queuePriority[] = { 1.0f };

	VkDeviceQueueCreateInfo deviceQueueInfo = {};
	deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueInfo.flags = 0;
	deviceQueueInfo.pNext = nullptr;
	deviceQueueInfo.pQueuePriorities = queuePriority;
	deviceQueueInfo.queueCount = 1;
	deviceQueueInfo.queueFamilyIndex = vulkanData.queueFamilyIndex;

	VkPhysicalDeviceFeatures deviceFeature = {};
	deviceFeature.samplerAnisotropy = VK_TRUE;
	
	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = nullptr;
	deviceInfo.flags = 0;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &deviceQueueInfo;

	deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtension.size());
	deviceInfo.ppEnabledExtensionNames = deviceExtension.data();

	deviceInfo.pEnabledFeatures = &deviceFeature;

	if (enableDeviceValidationLayer) {
		deviceInfo.enabledLayerCount = static_cast<uint32_t>(deviceValidationLayer.size());
		deviceInfo.ppEnabledLayerNames = deviceValidationLayer.data();
	}
	else {
		deviceInfo.enabledLayerCount = 0;
		deviceInfo.ppEnabledLayerNames = nullptr;
	}

	if (vkCreateDevice(vulkanData.physicalDevice, &deviceInfo, nullptr, &vulkanData.device) != VK_SUCCESS) {
		throw std::runtime_error("[ERROR] Failed to create device");
	}

	vulkanData.queue.resize(1);
	vkGetDeviceQueue(vulkanData.device, vulkanData.queueFamilyIndex, 0, &vulkanData.queue[0]);
}

VulkanEngineApplication::SwapChainSupportDetails VulkanEngineApplication::VulkanWindow::querySwapChainSupport(VkPhysicalDevice mDevice)
{
	VulkanEngineApplication::SwapChainSupportDetails mSwapChainProperties;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice, vulkanData.windowSurface, &mSwapChainProperties.mCapabilities);

	uint32_t mFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice, vulkanData.windowSurface, &mFormatCount, nullptr);
	if (mFormatCount) {
		mSwapChainProperties.mFormats.resize(mFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice, vulkanData.windowSurface, &mFormatCount, mSwapChainProperties.mFormats.data());
	}

	uint32_t mPresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice, vulkanData.windowSurface, &mPresentModeCount, nullptr);
	if (mPresentModeCount) {
		mSwapChainProperties.mPresentModes.resize(mPresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice, vulkanData.windowSurface, &mPresentModeCount, mSwapChainProperties.mPresentModes.data());
	}

	return mSwapChainProperties;
}

VkSurfaceFormatKHR VulkanEngineApplication::VulkanWindow::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &mAvailableFormats) {
	if (mAvailableFormats.size() == 1 && mAvailableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto & mAvailableFormat : mAvailableFormats) {
		if (mAvailableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && mAvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return mAvailableFormat;
		}
	}

	return mAvailableFormats[0];
}

VkPresentModeKHR VulkanEngineApplication::VulkanWindow::choosePresentMode(const std::vector<VkPresentModeKHR> &mAvailablePresentModes) {
	VkPresentModeKHR mBestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto &mAvailableMode : mAvailablePresentModes) {
		if (mAvailableMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return mAvailableMode;
		else if (mAvailableMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			mBestMode = mAvailableMode;
	}

	return mBestMode;
}

VkExtent2D VulkanEngineApplication::VulkanWindow::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &mCapabilities) {
	if (mCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return mCapabilities.currentExtent;
	} else {
		//int width, height;
		//glfwGetWindowSize(mWindow, &width, &height);

		VkExtent2D mWindowExtent = vulkanData.mSwapChainImageExtent;//{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		mWindowExtent.height = std::max(mCapabilities.minImageExtent.height,
			std::min(mCapabilities.maxImageExtent.height, mWindowExtent.height));
		mWindowExtent.width = std::max(mCapabilities.minImageExtent.width,
			std::min(mCapabilities.maxImageExtent.width, mWindowExtent.width));
		return mWindowExtent;
	}
}

void VulkanEngineApplication::VulkanWindow::createSwapchain(void) {
	SwapChainSupportDetails mSwapChainProperties = querySwapChainSupport(vulkanData.physicalDevice);

	VkSurfaceFormatKHR mSurfaceFormat = chooseSwapSurfaceFormat(mSwapChainProperties.mFormats);
	VkPresentModeKHR mSurfacePresentMode = choosePresentMode(mSwapChainProperties.mPresentModes);
	VkExtent2D mSurfaceExtent = chooseSwapExtent(mSwapChainProperties.mCapabilities);

	uint32_t mImageCount = mSwapChainProperties.mCapabilities.minImageCount + 1;
	if (mSwapChainProperties.mCapabilities.maxImageCount > 0) {
		mImageCount = std::min(mImageCount, mSwapChainProperties.mCapabilities.maxImageCount);
	}

	VkSwapchainCreateInfoKHR mSwapChainCreateStruct = {};
	mSwapChainCreateStruct.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	mSwapChainCreateStruct.surface = vulkanData.windowSurface;
	mSwapChainCreateStruct.minImageCount = mImageCount;
	mSwapChainCreateStruct.imageFormat = mSurfaceFormat.format;
	mSwapChainCreateStruct.imageColorSpace = mSurfaceFormat.colorSpace;
	mSwapChainCreateStruct.imageExtent = mSurfaceExtent;
	mSwapChainCreateStruct.presentMode = mSurfacePresentMode;
	mSwapChainCreateStruct.imageArrayLayers = 1;
	mSwapChainCreateStruct.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	mSwapChainCreateStruct.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	mSwapChainCreateStruct.queueFamilyIndexCount = 0;
	mSwapChainCreateStruct.pQueueFamilyIndices = nullptr;

	mSwapChainCreateStruct.preTransform = mSwapChainProperties.mCapabilities.currentTransform;
	mSwapChainCreateStruct.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	mSwapChainCreateStruct.clipped = VK_TRUE;
	mSwapChainCreateStruct.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(vulkanData.device, &mSwapChainCreateStruct, nullptr, &vulkanData.swapchain) != VK_SUCCESS) {
		throw std::runtime_error("[DGB]\tFailed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(vulkanData.device, vulkanData.swapchain, &mImageCount, nullptr);
	vulkanData.swapchainImage.resize(mImageCount);
	vkGetSwapchainImagesKHR(vulkanData.device, vulkanData.swapchain, &mImageCount, vulkanData.swapchainImage.data());

	vulkanData.mSwapChainImageFormat = mSwapChainCreateStruct.imageFormat;
	vulkanData.mSwapChainImageExtent = mSwapChainCreateStruct.imageExtent;
}

void VulkanEngineApplication::VulkanWindow::createSwapchainImageView(void) {
	vulkanData.swapchainImageView.resize(vulkanData.swapchainImage.size());
	for (size_t i = 0; i < vulkanData.swapchainImage.size(); ++i) {

		VkImageViewCreateInfo imageViewInfo = { };
		imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.pNext = nullptr;
		imageViewInfo.flags = 0;
		imageViewInfo.format = vulkanData.mSwapChainImageFormat;
		imageViewInfo.image = vulkanData.swapchainImage[i];
		imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		
		imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		
		imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewInfo.subresourceRange.baseArrayLayer = 0;
		imageViewInfo.subresourceRange.baseMipLevel = 0;
		imageViewInfo.subresourceRange.layerCount = 1;
		imageViewInfo.subresourceRange.levelCount = 1;

		if (vkCreateImageView(vulkanData.device, &imageViewInfo, nullptr, &vulkanData.swapchainImageView[i]) != VK_SUCCESS) {
			throw std::runtime_error("[DBG] Failed to create image view");
		}
	}
}

void VulkanEngineApplication::VulkanWindow::createRenderPass(void) {
	/*VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;*/

	VkAttachmentDescription mAttachmentDescription = {};
	mAttachmentDescription.format = vulkanData.mSwapChainImageFormat;
	mAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	mAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	mAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	mAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	mAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	mAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	mAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	//VkAttachmentReference depthAttachmentReference = {};
	//depthAttachmentReference.attachment = 1;
	//depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference mColorAttachmentReference = {};
	mColorAttachmentReference.attachment = 0;
	mColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription mSubpassStruct = {};
	mSubpassStruct.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	mSubpassStruct.colorAttachmentCount = 1;
	mSubpassStruct.pColorAttachments = &mColorAttachmentReference;
	mSubpassStruct.pDepthStencilAttachment = nullptr;//&depthAttachmentReference;

	std::array<VkAttachmentDescription, 1> attachment = { mAttachmentDescription };// , depthAttachment };

	VkRenderPassCreateInfo mRenderPassCreateInfoStruct = {};
	mRenderPassCreateInfoStruct.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	mRenderPassCreateInfoStruct.attachmentCount = static_cast<uint32_t>(attachment.size());
	mRenderPassCreateInfoStruct.pAttachments = attachment.data();
	mRenderPassCreateInfoStruct.subpassCount = 1;
	mRenderPassCreateInfoStruct.pSubpasses = &mSubpassStruct;

	VkSubpassDependency mDependency = {};
	mDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	mDependency.dstSubpass = 0;
	mDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;//VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
	mDependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	mDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	mDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

	mRenderPassCreateInfoStruct.dependencyCount = 1;
	mRenderPassCreateInfoStruct.pDependencies = &mDependency;

	if (vkCreateRenderPass(vulkanData.device, &mRenderPassCreateInfoStruct, nullptr, &vulkanData.swapchainRenderpass) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to create render pass!");
	}
}

void VulkanEngineApplication::VulkanWindow::createFramebuffer(void) {
	vulkanData.swapchainFramebuffer.resize(vulkanData.swapchainImage.size());

	for (size_t i = 0; i < vulkanData.swapchainFramebuffer.size(); ++i)
	{
		std::array<VkImageView, 1> mAttachments = {
			vulkanData.swapchainImageView[i]
			//depthImageView
		};

		VkFramebufferCreateInfo mFramebufferCreateInfoStruct = {};
		mFramebufferCreateInfoStruct.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		mFramebufferCreateInfoStruct.renderPass = vulkanData.swapchainRenderpass;
		mFramebufferCreateInfoStruct.attachmentCount = static_cast<uint32_t>(mAttachments.size());
		mFramebufferCreateInfoStruct.pAttachments = mAttachments.data();
		mFramebufferCreateInfoStruct.height = vulkanData.mSwapChainImageExtent.height;
		mFramebufferCreateInfoStruct.width = vulkanData.mSwapChainImageExtent.width;
		mFramebufferCreateInfoStruct.layers = 1;

		if (vkCreateFramebuffer(vulkanData.device, &mFramebufferCreateInfoStruct, nullptr, &vulkanData.swapchainFramebuffer[i]) != VK_SUCCESS) {
			throw std::runtime_error("[DBG]\tFailed to create framebuffer!");
		}
	}
}

void VulkanEngineApplication::VulkanWindow::createCommandPool(void) {
	VkCommandPoolCreateInfo mCommandPoolCreateInfoStruct = {};
	mCommandPoolCreateInfoStruct.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	mCommandPoolCreateInfoStruct.queueFamilyIndex = vulkanData.queueFamilyIndex;
	mCommandPoolCreateInfoStruct.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(vulkanData.device, &mCommandPoolCreateInfoStruct, nullptr, &vulkanData.commandPool) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to create command pool!");
	}
}

void VulkanEngineApplication::VulkanWindow::createCommandBuffer(void) {
	vulkanData.commandBuffer.resize(vulkanData.swapchainImage.size());

	VkCommandBufferAllocateInfo mCommandBufferAllocateInfoStruct = {};
	mCommandBufferAllocateInfoStruct.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	mCommandBufferAllocateInfoStruct.commandPool = vulkanData.commandPool;
	mCommandBufferAllocateInfoStruct.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	mCommandBufferAllocateInfoStruct.commandBufferCount = static_cast<uint32_t>(vulkanData.commandBuffer.size());

	if (vkAllocateCommandBuffers(vulkanData.device, &mCommandBufferAllocateInfoStruct, vulkanData.commandBuffer.data()) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to allocate command buffer!");
	}

	/*for (size_t i = 0; i < vulkanData.commandBuffer.size(); ++i)
	{
		VkCommandBufferBeginInfo mCommandBufferBeginInfo = {};
		mCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		mCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		mCommandBufferBeginInfo.pInheritanceInfo = nullptr;

		vkBeginCommandBuffer(vulkanData.commandBuffer[i], &mCommandBufferBeginInfo);

		drawImage->draw(vulkanData.commandBuffer[i], i);

		if (vkEndCommandBuffer(vulkanData.commandBuffer[i]) != VK_SUCCESS) {
			throw std::runtime_error("[DBG]\tFailed to record command buffer!");
		}
	}*/
}

void VulkanEngineApplication::VulkanWindow::createSemaphore(void) {
	VkSemaphoreCreateInfo mSemaphoreCreateInfo = {};
	mSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(vulkanData.device, &mSemaphoreCreateInfo, nullptr, &vulkanData.imageAvailable) != VK_SUCCESS ||
		vkCreateSemaphore(vulkanData.device, &mSemaphoreCreateInfo, nullptr, &vulkanData.renderFinish) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to create semaphores!");
	}
}

VkResult VulkanEngineApplication::VulkanWindow::createDebugReportCallbackEXT(VkInstance mxInstance,
	const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
	const VkAllocationCallbacks *mAllocators,
	VkDebugReportCallbackEXT *pCallback)
{
	auto mFunc = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
		vkGetInstanceProcAddr(mxInstance, "vkCreateDebugReportCallbackEXT"));
	if (mFunc != nullptr)
		return mFunc(mxInstance, pCreateInfo, mAllocators, pCallback);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanEngineApplication::VulkanWindow::destroyDebugReportCallbackEXT(
	VkInstance mxInstance,
	VkDebugReportCallbackEXT pCallback,
	const VkAllocationCallbacks * mAllocators)
{
	auto mFunc = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT> (
		vkGetInstanceProcAddr(mxInstance, "vkDestroyDebugReportCallbackEXT"));
	if (mFunc != nullptr)
		mFunc(mxInstance, pCallback, mAllocators);
	else
		throw std::runtime_error("[ERR]\t vkDestroyDebugReportCallbackEXT is not find!");
}

void VulkanEngineApplication::VulkanWindow::setupDebugReport(void)
{
	if (!enableInstanceValidationLayer)
		return;

	VkDebugReportCallbackCreateInfoEXT mDebugReportCallbackCreateInfoStruct = {};
	mDebugReportCallbackCreateInfoStruct.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	mDebugReportCallbackCreateInfoStruct.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;
	mDebugReportCallbackCreateInfoStruct.pfnCallback = debugCallback;
	mDebugReportCallbackCreateInfoStruct.pUserData = this;

	if (createDebugReportCallbackEXT(vulkanData.vulkanInstance, &mDebugReportCallbackCreateInfoStruct, nullptr, &mCallbackDebugReport) != VK_SUCCESS) {
		throw std::runtime_error("Failed to set up debug callback!");
	}
}

void VulkanEngineApplication::VulkanWindow::printApplicationInfo(void) {
	printInstanceExtensionInfo();
	printDeviceExtensionInfo();
	printInstanceValidationLayerInfo();
	printDeviceValidationLayerInfo();
	printPhisicalDevices();
}

void VulkanEngineApplication::VulkanWindow::printInstanceExtensionInfo(void) {

	std::cout << "[INFO] Instance Extension" << std::endl;
	std::vector<VkExtensionProperties> properties;
	uint32_t propertiesCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr);
	if (propertiesCount > 0) {
		properties.resize(propertiesCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, properties.data());

		for (auto v : properties) {
			std::cout << "[INFO] - " << v.extensionName << ", specVersion = " << v.specVersion << std::endl;
		}
	}
	std::cout << std::endl;
}

void VulkanEngineApplication::VulkanWindow::printDeviceExtensionInfo(void) {
}

void VulkanEngineApplication::VulkanWindow::printPhisicalDevices(void) {

	std::cout << "Enumerate Devices" << std::endl;
	std::vector<VkPhysicalDevice> devices;
	uint32_t deviceCount = 0u;
	vkEnumeratePhysicalDevices(vulkanData.vulkanInstance, &deviceCount, nullptr);
	if (deviceCount > 0) {
		devices.resize(deviceCount);
		vkEnumeratePhysicalDevices(vulkanData.vulkanInstance, &deviceCount, devices.data());
		for (const auto &d : devices) {

			VkPhysicalDeviceFeatures feature;
			vkGetPhysicalDeviceFeatures(d, &feature);
		
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(d, &properties);
			std::cout << "[INFO] " << "name: "<< properties.deviceName << ", tesselation shader usage " << feature.tessellationShader << std::endl;

		}
	}
	std::cout << std::endl;
}

void VulkanEngineApplication::VulkanWindow::printInstanceValidationLayerInfo(void) {

	std::cout << "[INFO] Instance Validation Layer" << std::endl;
	std::vector<VkLayerProperties> properties;
	uint32_t propertiesCount = 0;
	vkEnumerateInstanceLayerProperties(&propertiesCount, nullptr);
	if (propertiesCount > 0) {
		properties.resize(propertiesCount);
		vkEnumerateInstanceLayerProperties(&propertiesCount, properties.data());

		for (auto v : properties) {
			std::cout << "[INFO] - " << v.layerName << ", specVersion = " << v.specVersion << std::endl;
		}
	}
	std::cout << std::endl;
}

void VulkanEngineApplication::VulkanWindow::printDeviceValidationLayerInfo(void) {
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanEngineApplication::VulkanWindow::debugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char * layerPrefix,
	const char * msg,
	void * userData)
{
	std::cout << "[DBG]\tValidation Layer: " << msg << std::endl;

	return VK_FALSE;
}
