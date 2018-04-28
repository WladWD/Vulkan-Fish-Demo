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
		uint32_t mImageIndex;
		//////////////////////////////////////////////////
		VkInstance vulkanInstance;
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		//////////////////////////////////////////////////
		std::vector<VkQueue> queue;
		int32_t queueFamilyIndex;
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
	};
}
