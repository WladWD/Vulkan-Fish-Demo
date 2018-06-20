#include "VulkanHeader.h"
#pragma once

namespace ImageManager {
	struct ImageData {
		VkImage image;
		VkDeviceMemory imageMemory;

		VkImageView imageView;
		VkFormat imageFormat;
		VkExtent3D imageExtent;

		uint32_t levelCount;
		uint32_t layerCount;

		ImageData(): image(VK_NULL_HANDLE), 
			imageMemory(VK_NULL_HANDLE),
			imageView(VK_NULL_HANDLE), 
			imageFormat(VK_FORMAT_UNDEFINED),
			levelCount(0), 
			layerCount(0) { }
	};

}