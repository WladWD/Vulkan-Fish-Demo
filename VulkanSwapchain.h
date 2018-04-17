#include "InitStruct.h"
#pragma once

namespace VulkanEngineApplication
{
	class VulkanSwapchain
	{
		/////////////////////////////////////////////////////////////////////////////////////
		VulkanEngineApplication::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice mDevice);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &mAvailableFormats);
		VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &mAvailablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &mCapabilities);
		/////////////////////////////////////////////////////////////////////////////////////
		VulkanEngineApplication::VulkanData *vulkanData;
		/////////////////////////////////////////////////////////////////////////////////////
		void createSwapchain(void);
		void createSwapchainImageView(void);

		void free(void);
		/////////////////////////////////////////////////////////////////////////////////////
	public:
		VulkanSwapchain(const VulkanEngineApplication::VulkanData *vulkanData);
		~VulkanSwapchain();
		/////////////////////////////////////////////////////////////////////////////////////
		void initialize(void);
		void resize(void);
		/////////////////////////////////////////////////////////////////////////////////////
	};
}

