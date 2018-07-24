#include "VulkanSwapchain.h"


VulkanEngineApplication::VulkanSwapchain::VulkanSwapchain(const VulkanEngineApplication::VulkanData * vulkanData) : vulkanData(const_cast<VulkanEngineApplication::VulkanData *>(vulkanData)) {
}

VulkanEngineApplication::VulkanSwapchain::~VulkanSwapchain() {
	free();
}

VulkanEngineApplication::SwapChainSupportDetails VulkanEngineApplication::VulkanSwapchain::querySwapChainSupport(VkPhysicalDevice mDevice)
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

VkSurfaceFormatKHR VulkanEngineApplication::VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &mAvailableFormats) {
	if (mAvailableFormats.size() == 1 && mAvailableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto & mAvailableFormat : mAvailableFormats) {
		if (mAvailableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && mAvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return mAvailableFormat;
		}
	}

	return mAvailableFormats[0];
}

VkPresentModeKHR VulkanEngineApplication::VulkanSwapchain::choosePresentMode(const std::vector<VkPresentModeKHR> &mAvailablePresentModes) {
	VkPresentModeKHR mBestMode = VK_PRESENT_MODE_FIFO_KHR;

	return mBestMode;

	for (const auto &mAvailableMode : mAvailablePresentModes) {
		if (mAvailableMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return mAvailableMode;
		else if (mAvailableMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			mBestMode = mAvailableMode;
	}

	return mBestMode;
}

VkExtent2D VulkanEngineApplication::VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &mCapabilities) {
	if (mCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return mCapabilities.currentExtent;
	}
	else {

		VkExtent2D mWindowExtent = vulkanData->mSwapChainImageExtent;

		mWindowExtent.height = std::max(mCapabilities.minImageExtent.height,
			std::min(mCapabilities.maxImageExtent.height, mWindowExtent.height));
		mWindowExtent.width = std::max(mCapabilities.minImageExtent.width,
			std::min(mCapabilities.maxImageExtent.width, mWindowExtent.width));
		return mWindowExtent;
	}
}

VkFormat VulkanEngineApplication::VulkanSwapchain::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags feature)
{
	for (VkFormat format : candidates) {
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(vulkanData->physicalDevice, format, &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR && ((properties.linearTilingFeatures & feature) == feature))
			return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && ((properties.optimalTilingFeatures & feature) == feature))
			return format;
	}

	throw std::runtime_error("Failed to find compatible format");
}

VkFormat VulkanEngineApplication::VulkanSwapchain::findDepthFormat(void) {
	return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool VulkanEngineApplication::VulkanSwapchain::hasStensilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VulkanEngineApplication::VulkanSwapchain::createDepthImage() {
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.arrayLayers = 1;
	imageInfo.extent.width = vulkanData->mSwapChainImageExtent.width;
	imageInfo.extent.height = vulkanData->mSwapChainImageExtent.height;
	imageInfo.extent.depth = 1;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.flags = 0;
	imageInfo.mipLevels = 1;
	imageInfo.format = vulkanData->depthFormat;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	VulkanInitialize::createImage(vulkanData, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &imageInfo, vulkanData->depthBuffer, vulkanData->depthMemory);
}

void VulkanEngineApplication::VulkanSwapchain::createDepthImageView() {
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = vulkanData->depthBuffer;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = vulkanData->depthFormat;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.layerCount = 1;
	viewInfo.subresourceRange.levelCount = 1;

	if (hasStensilComponent(viewInfo.format)) {
		viewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}


	if (vkCreateImageView(vulkanData->device, &viewInfo, nullptr, &vulkanData->depthBufferView) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed create image view");
	}
}

void VulkanEngineApplication::VulkanSwapchain::createDepthBuffer(void) {
	vulkanData->depthFormat = findDepthFormat();
	createDepthImage();
	createDepthImageView();

	VkCommandBuffer commandBuffer = VulkanInitialize::beginSingleTimeCommand(vulkanData, vulkanData->commandPool);
	VkImageAspectFlags aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	if (hasStensilComponent(vulkanData->depthFormat)) {
		aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	VulkanInitialize::transitionImageLayout(vulkanData->depthBuffer,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 
		0, 0, 
		0, 0, 
		aspect,
		commandBuffer);
	VulkanInitialize::endSingleTimeCommand(vulkanData, vulkanData->commandPool, commandBuffer);
}

void VulkanEngineApplication::VulkanSwapchain::createSwapchain(void) {
	SwapChainSupportDetails mSwapChainProperties = querySwapChainSupport(vulkanData->physicalDevice);

	VkSurfaceFormatKHR mSurfaceFormat = chooseSwapSurfaceFormat(mSwapChainProperties.mFormats);
	VkPresentModeKHR mSurfacePresentMode = choosePresentMode(mSwapChainProperties.mPresentModes);
	VkExtent2D mSurfaceExtent = chooseSwapExtent(mSwapChainProperties.mCapabilities);

	uint32_t mImageCount = mSwapChainProperties.mCapabilities.minImageCount + 1;
	if (mSwapChainProperties.mCapabilities.maxImageCount > 0) {
		mImageCount = std::min(mImageCount, mSwapChainProperties.mCapabilities.maxImageCount);
	}

	VkSwapchainCreateInfoKHR mSwapChainCreateStruct = {};
	mSwapChainCreateStruct.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	mSwapChainCreateStruct.surface = vulkanData->windowSurface;
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

	if (vkCreateSwapchainKHR(vulkanData->device, &mSwapChainCreateStruct, nullptr, &vulkanData->swapchain) != VK_SUCCESS) {
		throw std::runtime_error("[DGB]\tFailed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(vulkanData->device, vulkanData->swapchain, &mImageCount, nullptr);
	vulkanData->swapchainImage.resize(mImageCount);
	vkGetSwapchainImagesKHR(vulkanData->device, vulkanData->swapchain, &mImageCount, vulkanData->swapchainImage.data());

	vulkanData->mSwapChainImageFormat = mSwapChainCreateStruct.imageFormat;
	vulkanData->mSwapChainImageExtent = mSwapChainCreateStruct.imageExtent;
}

void VulkanEngineApplication::VulkanSwapchain::createSwapchainImageView(void) {
	vulkanData->swapchainImageView.resize(vulkanData->swapchainImage.size());
	for (size_t i = 0; i < vulkanData->swapchainImage.size(); ++i) {

		VkImageViewCreateInfo imageViewInfo = {};
		imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.pNext = nullptr;
		imageViewInfo.flags = 0;
		imageViewInfo.format = vulkanData->mSwapChainImageFormat;
		imageViewInfo.image = vulkanData->swapchainImage[i];
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

		if (vkCreateImageView(vulkanData->device, &imageViewInfo, nullptr, &vulkanData->swapchainImageView[i]) != VK_SUCCESS) {
			throw std::runtime_error("[DBG] Failed to create image view");
		}
	}
}

void VulkanEngineApplication::VulkanSwapchain::free(void) {

	vkDestroyImageView(vulkanData->device, vulkanData->depthBufferView,  nullptr);
	vkDestroyImage(vulkanData->device, vulkanData->depthBuffer, nullptr);
	vkFreeMemory(vulkanData->device, vulkanData->depthMemory, nullptr);

	for (size_t i = 0; i < vulkanData->swapchainImageView.size(); ++i)
		vkDestroyImageView(vulkanData->device, vulkanData->swapchainImageView[i], nullptr);

	vkDestroySwapchainKHR(vulkanData->device, vulkanData->swapchain, nullptr);
}

void VulkanEngineApplication::VulkanSwapchain::initialize(void) {
	createSwapchain();
	createSwapchainImageView();
}

void VulkanEngineApplication::VulkanSwapchain::resize(void) {
	free();
	initialize();
	createDepthBuffer();
}
