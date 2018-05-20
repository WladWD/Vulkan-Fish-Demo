#include "InitStruct.h"

#pragma once
namespace VulkanInitialize {
	uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkCommandBuffer beginSingleTimeCommand(const VulkanEngineApplication::VulkanData * vulkanData, VkCommandPool commandPool);
	void endSingleTimeCommand(const VulkanEngineApplication::VulkanData * vulkanData, VkCommandPool commandPool, VkCommandBuffer commandBuffer);

	void transitionImageLayout(VkImage image, VkFormat format,
		VkImageLayout oldLayout, VkImageLayout newLayout,
		VkAccessFlags srcAccess, VkAccessFlags dstAccess,
		VkPipelineStageFlags srcStageMack, VkPipelineStageFlags dstStageMack,
		VkImageAspectFlags aspect,
		VkCommandBuffer commandBuffer);

	void createBuffer(const VulkanEngineApplication::VulkanData * vulkanData,
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer &buffer,
		VkDeviceMemory& bufferMemory);
	void createInitializeDeviceLocalBuffer(const VulkanEngineApplication::VulkanData * vulkanData,
		VkCommandPool commandPool,
		VkBufferUsageFlags bufferUsage,
		VkMemoryPropertyFlags properties,
		VkDeviceSize bufferSize,
		const void *bufferData,
		VkBuffer &buffer,
		VkDeviceMemory& bufferMemory);

	void createImage2D(const VulkanEngineApplication::VulkanData * vulkanData,
		VkMemoryPropertyFlags memoryFlag,
		const VkImageCreateInfo *imageInfo,
		VkImage &image,
		VkDeviceMemory &imageMemory);

	void initializeImage2D(const VulkanEngineApplication::VulkanData * vulkanData,
		VkCommandPool commandPool,
		const void * imageData,
		VkDeviceSize imageDataSize,
		VkExtent3D extent,
		VkFormat format,
		VkImageAspectFlags aspectFlag,
		uint32_t layer_count,
		VkImage image);
}

