#include "VulkanWindow.h"

VulkanEngineApplication::VulkanWindow::VulkanWindow(JNIEnv *env, jobject rmng) {
	//init engine data;
	engine = new Engine::VulkanEngine(&vulkanData, env, rmng);
}

VulkanEngineApplication::VulkanWindow::~VulkanWindow() {
	
	delete engine;

	vkDestroySurfaceKHR(vulkanInstance, vulkanData.windowSurface, nullptr);

	destroyDebugReportCallbackEXT(vulkanInstance, mCallbackDebugReport, nullptr);

	vkDestroyInstance(vulkanInstance, nullptr);

	glfwDestroyWindow(mWindow);

	glfwTerminate();
}

bool VulkanEngineApplication::VulkanWindow::windowShoudClose(void) {
	return !!glfwWindowShouldClose(mWindow);
}

void VulkanEngineApplication::VulkanWindow::initialize(uint32_t width, uint32_t height) {

	vulkanData.height = height;
	vulkanData.width = width;


	initWindow(width, height, "Vulkan Application");
	initVulkan();

#ifdef _DEBUG
	printApplicationInfo();
#endif

	engine->initialize();
}

void VulkanEngineApplication::VulkanWindow::resize(uint32_t width, uint32_t height) {
	vulkanData.height = height;
	vulkanData.width = width;

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
	engine->draw();
}

void VulkanEngineApplication::VulkanWindow::initWindow(int32_t width, int32_t height, const char * sTitle) {

#ifndef _ANDROID_

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	mWindow = glfwCreateWindow(width, height, sTitle, nullptr, nullptr);

	glfwSetWindowUserPointer(mWindow, this);
	glfwSetWindowSizeCallback(mWindow, OnWindowResize);

#endif
}

void VulkanEngineApplication::VulkanWindow::initVulkan(void) {
	createInstance();
	setupDebugReport();
	createWindowSurface();
	createDevice();
	createSwapchain();
	createSwapchainImageView();
	createRenderPass();
	createFramebuffer();
}

void VulkanEngineApplication::VulkanWindow::createInstance(void) {

	VkApplicationInfo appInfo = { };
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.pApplicationName = "FishDemo";
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

	addInstanceRequiredExtension();
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtension.size());
	instanceInfo.ppEnabledExtensionNames = instanceExtension.data();

	if (vkCreateInstance(&instanceInfo, nullptr, &vulkanInstance) != VK_SUCCESS) {
		throw std::runtime_error("[Error] Failed create instace!");
	}
}

void VulkanEngineApplication::VulkanWindow::addInstanceRequiredExtension(void) {

	uint32_t glfwExtensionCount = 0;
	const char **glfwExtension;

	glfwExtension = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		instanceExtension.push_back(glfwExtension[i]);

	if (enableInstanceValidationLayer)
		instanceExtension.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
}

void VulkanEngineApplication::VulkanWindow::selectPhisicalDevice(void) {
	std::vector<VkPhysicalDevice> phisicalDevices;
	uint32_t deviceCount = 0u;
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);
	if (deviceCount > 0) {
		phisicalDevices.resize(deviceCount);
		vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, phisicalDevices.data());
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

void VulkanEngineApplication::VulkanWindow::createWindowSurface(void) {
#ifndef _ANDROID_

	if (glfwCreateWindowSurface(vulkanInstance, mWindow, nullptr, &vulkanData.windowSurface) != VK_SUCCESS) {
		throw std::runtime_error("[Error] Failed to create window surface!");
	}

#endif
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
		int width, height;
		glfwGetWindowSize(mWindow, &width, &height);

		VkExtent2D mWindowExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

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
	mSwapChainImageExtent = mSwapChainCreateStruct.imageExtent;
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
		mFramebufferCreateInfoStruct.height = mSwapChainImageExtent.height;
		mFramebufferCreateInfoStruct.width = mSwapChainImageExtent.width;
		mFramebufferCreateInfoStruct.layers = 1;

		if (vkCreateFramebuffer(vulkanData.device, &mFramebufferCreateInfoStruct, nullptr, &vulkanData.swapchainFramebuffer[i]) != VK_SUCCESS) {
			throw std::runtime_error("[DBG]\tFailed to create framebuffer!");
		}
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

	if (createDebugReportCallbackEXT(vulkanInstance, &mDebugReportCallbackCreateInfoStruct, nullptr, &mCallbackDebugReport) != VK_SUCCESS)
		throw std::runtime_error("Failed to set up debug callback!");
}

void VulkanEngineApplication::VulkanWindow::OnWindowResize(GLFWwindow * mWindow, int width, int height)
{
	if (!width || !height)
		return;

	VulkanEngineApplication::VulkanWindow *mApplication = static_cast<VulkanEngineApplication::VulkanWindow *>(glfwGetWindowUserPointer(mWindow));
	//mApplication->RecreateSwapChain();
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
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);
	if (deviceCount > 0) {
		devices.resize(deviceCount);
		vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, devices.data());
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
