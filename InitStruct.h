#include "VulkanHeader.h"
#include "SamplersContainer.h"

#pragma once
namespace VulkanEngineApplication
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR mCapabilities;
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
	};

	struct VulkanData {
		//////////////////////////////////////////////////
		uint32_t mImageIndex;
		//////////////////////////////////////////////////
		VkInstance vulkanInstance;
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		//////////////////////////////////////////////////
		std::vector<VkQueue> graphicsQueue;
		int32_t graphicsQueueFamilyIndex;
		//present_queue_family
		//game_queue_family
		//////////////////////////////////////////////////
		VkSurfaceKHR windowSurface;
		VkSwapchainKHR swapchain;
		VkFormat mSwapChainImageFormat;
		VkExtent2D mSwapChainImageExtent;
		//////////////////////////////////////////////////
		std::vector<VkImage> swapchainImage;
		std::vector<VkImageView> swapchainImageView;
		//////////////////////////////////////////////////
		std::vector<VkFramebuffer> swapchainFramebuffer;
		VkRenderPass swapchainRenderpass;
		//////////////////////////////////////////////////
		VkSemaphore imageAvailable, renderFinish;
		//////////////////////////////////////////////////
		std::vector<VkCommandBuffer> commandBuffer;
		VkCommandPool commandPool;
		//////////////////////////////////////////////////
		//Samplers::SamplersContainer *samplers;
	};
}
