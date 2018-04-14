#include "VulkanWindow.h"

VulkanEngineApplication::VulkanWindow::VulkanWindow(int32_t width, int32_t height, const char * sTitle) {
	initWindow(width, height, sTitle);
	initVulkan();

#ifdef _DEBUG
	printApplicationInfo();
#endif

}

VulkanEngineApplication::VulkanWindow::~VulkanWindow() {
	
	vkDestroySurfaceKHR(vulkanInstance, windowSurface, nullptr);

	destroyDebugReportCallbackEXT(vulkanInstance, mCallbackDebugReport, nullptr);

	vkDestroyInstance(vulkanInstance, nullptr);

	glfwDestroyWindow(mWindow);

	glfwTerminate();
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
	createSwapShain();
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
				physicalDevice = device;
				this->queueFamilyIndex = queueFamilyIndex;
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

		vkGetPhysicalDeviceSurfaceSupportKHR(mDevice, mCurrentQueueFamilyIndex, windowSurface, &bPhysicalDeviceSurfaceSupport);

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

	if (glfwCreateWindowSurface(vulkanInstance, mWindow, nullptr, &windowSurface) != VK_SUCCESS) {
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
	deviceQueueInfo.queueFamilyIndex = queueFamilyIndex;

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

	if (vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("[ERROR] Failed to create device");
	}

	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue[0]);
}

VulkanEngineApplication::SwapChainSupportDetails VulkanEngineApplication::VulkanWindow::querySwapChainSupport(VkPhysicalDevice mDevice)
{
	VulkanEngineApplication::SwapChainSupportDetails mSwapChainProperties;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice, windowSurface, &mSwapChainProperties.mCapabilities);

	uint32_t mFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice, windowSurface, &mFormatCount, nullptr);
	if (mFormatCount) {
		mSwapChainProperties.mFormats.resize(mFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice, windowSurface, &mFormatCount, mSwapChainProperties.mFormats.data());
	}

	uint32_t mPresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice, windowSurface, &mPresentModeCount, nullptr);
	if (mPresentModeCount) {
		mSwapChainProperties.mPresentModes.resize(mPresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice, windowSurface, &mPresentModeCount, mSwapChainProperties.mPresentModes.data());
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

void VulkanEngineApplication::VulkanWindow::createSwapShain(void) {
	SwapChainSupportDetails mSwapChainProperties = querySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR mSurfaceFormat = chooseSwapSurfaceFormat(mSwapChainProperties.mFormats);
	VkPresentModeKHR mSurfacePresentMode = choosePresentMode(mSwapChainProperties.mPresentModes);
	VkExtent2D mSurfaceExtent = chooseSwapExtent(mSwapChainProperties.mCapabilities);

	uint32_t mImageCount = mSwapChainProperties.mCapabilities.minImageCount + 1;
	if (mSwapChainProperties.mCapabilities.maxImageCount > 0) {
		mImageCount = std::min(mImageCount, mSwapChainProperties.mCapabilities.maxImageCount);
	}

	VkSwapchainCreateInfoKHR mSwapChainCreateStruct = {};
	mSwapChainCreateStruct.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	mSwapChainCreateStruct.surface = windowSurface;
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

	if (vkCreateSwapchainKHR(device, &mSwapChainCreateStruct, nullptr, &swapchain) != VK_SUCCESS) {
		throw std::runtime_error("[DGB]\tFailed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, swapchain, &mImageCount, nullptr);
	swapchainImage.resize(mImageCount);
	vkGetSwapchainImagesKHR(device, swapchain, &mImageCount, swapchainImage.data());

	mSwapChainImageFormat = mSwapChainCreateStruct.imageFormat;
	mSwapChainImageExtent = mSwapChainCreateStruct.imageExtent;
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

void VulkanEngineApplication::VulkanWindow::printDeviceValidationLayerInfo(void)
{
}


void VulkanEngineApplication::VulkanWindow::run() {
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
