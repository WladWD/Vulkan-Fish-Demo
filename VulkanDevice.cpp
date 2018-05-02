#include "VulkanDevice.h"



VulkanEngineApplication::VulkanDevice::VulkanDevice(const VulkanEngineApplication::VulkanData * vulkanData) : vulkanData(const_cast<VulkanEngineApplication::VulkanData *>(vulkanData)) {

}

VulkanEngineApplication::VulkanDevice::~VulkanDevice() {
	vkDestroyDevice(vulkanData->device, nullptr);
}

void VulkanEngineApplication::VulkanDevice::selectPhisicalDevice(void) {
	std::vector<VkPhysicalDevice> phisicalDevices;
	uint32_t deviceCount = 0u;
	vkEnumeratePhysicalDevices(vulkanData->vulkanInstance, &deviceCount, nullptr);
	if (deviceCount > 0) {
		phisicalDevices.resize(deviceCount);
		vkEnumeratePhysicalDevices(vulkanData->vulkanInstance, &deviceCount, phisicalDevices.data());
		for (const auto &device : phisicalDevices) {
			int32_t graphicsQueueFamilyIndex = -1;
			if (isDeviceSuitable(device, graphicsQueueFamilyIndex)) {
				vulkanData->physicalDevice = device;
				vulkanData->graphicsQueueFamilyIndex = graphicsQueueFamilyIndex;
				break;
			}
		}
	}
}

VulkanEngineApplication::SwapChainSupportDetails VulkanEngineApplication::VulkanDevice::querySwapChainSupport(VkPhysicalDevice mDevice)
{
	VulkanEngineApplication::SwapChainSupportDetails mSwapChainProperties;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice, vulkanData->windowSurface, &mSwapChainProperties.mCapabilities);

	uint32_t mFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice, vulkanData->windowSurface, &mFormatCount, nullptr);
	if (mFormatCount) {
		mSwapChainProperties.mFormats.resize(mFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice, vulkanData->windowSurface, &mFormatCount, mSwapChainProperties.mFormats.data());
	}

	uint32_t mPresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice, vulkanData->windowSurface, &mPresentModeCount, nullptr);
	if (mPresentModeCount) {
		mSwapChainProperties.mPresentModes.resize(mPresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice, vulkanData->windowSurface, &mPresentModeCount, mSwapChainProperties.mPresentModes.data());
	}

	return mSwapChainProperties;
}

int32_t VulkanEngineApplication::VulkanDevice::findQueueFamily(VkPhysicalDevice mDevice)
{
	uint32_t mQueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(mDevice, &mQueueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> mQueueFamilyProperties(mQueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mDevice, &mQueueFamilyCount, mQueueFamilyProperties.data());

	int32_t mCurrentQueueFamilyIndex = 0;
	bool mFind = false;
	for (const auto &mQueueFamily : mQueueFamilyProperties) {
		VkBool32 bPhysicalDeviceSurfaceSupport = false;

		vkGetPhysicalDeviceSurfaceSupportKHR(mDevice, mCurrentQueueFamilyIndex, vulkanData->windowSurface, &bPhysicalDeviceSurfaceSupport);

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

bool VulkanEngineApplication::VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice mDevice)
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

bool VulkanEngineApplication::VulkanDevice::isDeviceSuitable(VkPhysicalDevice mDevice, int32_t &mDeviceQueueFamilyIndex) {
	VkPhysicalDeviceProperties mDeviceProperties;
	VkPhysicalDeviceFeatures mDeviceFeatures;
	vkGetPhysicalDeviceProperties(mDevice, &mDeviceProperties);
	vkGetPhysicalDeviceFeatures(mDevice, &mDeviceFeatures);

	mDeviceQueueFamilyIndex = findQueueFamily(mDevice);

	bool mDeviceAvailableExtensionSupport = checkDeviceExtensionSupport(mDevice);
	bool mSwapChainAdequate = false;
	if (mDeviceAvailableExtensionSupport) {
		auto mSwapChainProperties = querySwapChainSupport(mDevice);
		mSwapChainAdequate = !mSwapChainProperties.mFormats.empty() && !mSwapChainProperties.mPresentModes.empty();
	}

	return	(mDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || mDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) &&
		mDeviceAvailableExtensionSupport &&
		mSwapChainAdequate &&
		mDeviceFeatures.geometryShader &&
		mDeviceFeatures.tessellationShader &&
		//mDeviceFeatures.samplerAnisotropy &&
		(mDeviceQueueFamilyIndex >= 0);
}

void VulkanEngineApplication::VulkanDevice::createDevice(void) {
	selectPhisicalDevice();

	const float queuePriority[] = { 1.0f };

	VkDeviceQueueCreateInfo deviceQueueInfo = {};
	deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueInfo.flags = 0;
	deviceQueueInfo.pNext = nullptr;
	deviceQueueInfo.pQueuePriorities = queuePriority;
	deviceQueueInfo.queueCount = 1;
	deviceQueueInfo.queueFamilyIndex = vulkanData->graphicsQueueFamilyIndex;

	VkPhysicalDeviceFeatures deviceFeature = {};
	//deviceFeature.samplerAnisotropy = VK_TRUE;

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

    VkResult result;
	if ((result = vkCreateDevice(vulkanData->physicalDevice, &deviceInfo, nullptr, &vulkanData->device)) != VK_SUCCESS) {
		throw std::runtime_error("[ERROR] Failed to create device");
	}

	vulkanData->graphicsQueue.resize(1);
	vkGetDeviceQueue(vulkanData->device, vulkanData->graphicsQueueFamilyIndex, 0, &vulkanData->graphicsQueue[0]);
}

void VulkanEngineApplication::VulkanDevice::initialize(void) {
	createDevice();
}
