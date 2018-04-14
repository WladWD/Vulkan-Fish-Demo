#include "VulkanHeader.h"

#pragma once
namespace VulkanEngineApplication
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR mCapabilities;
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
	};

	struct UniformBuffer {
		glm::mat4 projView;
	};

	struct VulkanData {
		//////////////////////////////////////////////////
		int32_t width, height;
		//////////////////////////////////////////////////
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		//////////////////////////////////////////////////
		std::vector<VkQueue> queue;
		int32_t queueFamilyIndex;
		//////////////////////////////////////////////////
		VkSurfaceKHR windowSurface;
		VkSwapchainKHR swapchain;
		VkFormat mSwapChainImageFormat;
		std::vector<VkImage> swapchainImage;
		std::vector<VkImageView> swapchainImageView;
		//////////////////////////////////////////////////
		std::vector<VkFramebuffer> swapchainFramebuffer;
		VkRenderPass swapchainRenderpass;
		//////////////////////////////////////////////////
	};
}
