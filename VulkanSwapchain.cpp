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
}
