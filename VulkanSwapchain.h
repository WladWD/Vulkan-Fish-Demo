#include "InitStruct.h"
#include "VulkanCreater.h"
#pragma once

namespace VulkanEngineApplication
{
	class VulkanSwapchain {
		VulkanEngineApplication::VulkanData *vulkanData;
		
		VulkanEngineApplication::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice mDevice);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &mAvailableFormats);
		VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &mAvailablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &mCapabilities);
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags feature);
		VkFormat findDepthFormat(void);

		bool hasStensilComponent(VkFormat format);
		void createDepthImage(void);
		void createDepthImageView(void);
		void createSwapchain(void);
		void createSwapchainImageView(void);
		void free(void);
	public:
		VulkanSwapchain(const VulkanEngineApplication::VulkanData *vulkanData);
		~VulkanSwapchain();

		void createDepthBuffer(void);
		void initialize(void);
		void resize(void);
	};
}

