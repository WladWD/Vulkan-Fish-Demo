#include "VulkanCreater.h"

uint32_t VulkanInitialize::findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if ((typeFilter & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties))
			return i;
	}

	throw std::runtime_error("Failed to find suitable memory type!");
	return 0;
}

VkCommandBuffer VulkanInitialize::beginSingleTimeCommand(const VulkanEngineApplication::VulkanData * vulkanData, VkCommandPool commandPool) {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandBufferCount = 1;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VkCommandBuffer copyCommandBuffer;
	vkAllocateCommandBuffers(vulkanData->device, &allocInfo, &copyCommandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(copyCommandBuffer, &beginInfo);

	return copyCommandBuffer;
}

void VulkanInitialize::endSingleTimeCommand(const VulkanEngineApplication::VulkanData * vulkanData, VkCommandPool commandPool, VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(vulkanData->graphicsQueue[0], 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(vulkanData->graphicsQueue[0]);

	vkFreeCommandBuffers(vulkanData->device, commandPool, 1, &commandBuffer);
}

void VulkanInitialize::transitionImageLayout(VkImage image, VkFormat format,
	VkImageLayout oldLayout, VkImageLayout newLayout,
	VkAccessFlags srcAccess, VkAccessFlags dstAccess,
	VkPipelineStageFlags srcStageMack, VkPipelineStageFlags dstStageMack,
	VkImageAspectFlags aspect,
	VkCommandBuffer commandBuffer)
{
	VkImageMemoryBarrier imageBarrier = {};
	imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageBarrier.oldLayout = oldLayout;
	imageBarrier.newLayout = newLayout;
	imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.image = image;
	imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBarrier.subresourceRange.baseArrayLayer = 0;
	imageBarrier.subresourceRange.baseMipLevel = 0;
	imageBarrier.subresourceRange.layerCount = 1;
	imageBarrier.subresourceRange.levelCount = 1;
	imageBarrier.dstAccessMask = 0;
	imageBarrier.srcAccessMask = 0;

	imageBarrier.srcAccessMask = srcAccess;
	imageBarrier.dstAccessMask = dstAccess;

	vkCmdPipelineBarrier(
		commandBuffer,
		srcStageMack,
		dstStageMack,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageBarrier);
}

void VulkanInitialize::createBuffer(const VulkanEngineApplication::VulkanData * vulkanData,
	VkDeviceSize size, VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer & buffer,
	VkDeviceMemory & bufferMemory) {
	VkBufferCreateInfo mBufferCreateInfo = {};
	mBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	mBufferCreateInfo.size = size;
	mBufferCreateInfo.usage = usage;
	mBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	mBufferCreateInfo.flags = 0;

	if (vkCreateBuffer(vulkanData->device, &mBufferCreateInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to create vertex buffer");
	}

	VkMemoryRequirements mMemoryRequiremets;
	vkGetBufferMemoryRequirements(vulkanData->device, buffer, &mMemoryRequiremets);

	VkMemoryAllocateInfo memAllocateInfo = {};
	memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocateInfo.allocationSize = mMemoryRequiremets.size;
	memAllocateInfo.memoryTypeIndex = findMemoryType(vulkanData->physicalDevice, mMemoryRequiremets.memoryTypeBits, properties);

	if (vkAllocateMemory(vulkanData->device, &memAllocateInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(vulkanData->device, buffer, bufferMemory, 0);
}

void VulkanInitialize::createInitializeDeviceLocalBuffer(const VulkanEngineApplication::VulkanData * vulkanData,
	VkCommandPool commandPool,
	VkBufferUsageFlags bufferUsage,
	VkMemoryPropertyFlags properties,
	VkDeviceSize bufferSize,
	const void *bufferData,
	VkBuffer &buffer,
	VkDeviceMemory& bufferMemory) {

	VkCommandBuffer commandBuffer = beginSingleTimeCommand(vulkanData, commandPool);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	VulkanInitialize::createBuffer(vulkanData, bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingMemory);

	void *stagingBufferData;
	vkMapMemory(vulkanData->device, stagingMemory, 0, bufferSize, 0, &stagingBufferData);
	memcpy(stagingBufferData, bufferData, static_cast<size_t>(bufferSize));
	vkUnmapMemory(vulkanData->device, stagingMemory);

	VulkanInitialize::createBuffer(vulkanData, bufferSize,
		bufferUsage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		properties,
		buffer, bufferMemory);

	VkBufferCopy copyRegion = {};
	copyRegion.dstOffset = 0;
	copyRegion.srcOffset = 0;
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(commandBuffer, stagingBuffer, buffer, 1, &copyRegion);

	endSingleTimeCommand(vulkanData, commandPool, commandBuffer);

	vkDestroyBuffer(vulkanData->device, stagingBuffer, nullptr);
	vkFreeMemory(vulkanData->device, stagingMemory, nullptr);
}

void VulkanInitialize::createImage2D(const VulkanEngineApplication::VulkanData * vulkanData,
	VkMemoryPropertyFlags memoryFlag,
	const VkImageCreateInfo *imageInfo,
	VkImage & image,
	VkDeviceMemory & imageMemory) {

	VkResult result;
	if ((result = vkCreateImage(vulkanData->device, imageInfo, nullptr, &image)) != VK_SUCCESS) {
		std::runtime_error("[DBG] Failed to create texture");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(vulkanData->device, image, &memoryRequirements);

	VkMemoryAllocateInfo allocationInfo = {};
	allocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocationInfo.allocationSize = memoryRequirements.size;
	allocationInfo.memoryTypeIndex = findMemoryType(vulkanData->physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
	allocationInfo.pNext = nullptr;

	if (vkAllocateMemory(vulkanData->device, &allocationInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to create texture");
	}

	vkBindImageMemory(vulkanData->device, image, imageMemory, 0);
}

void VulkanInitialize::initializeImage2D(const VulkanEngineApplication::VulkanData * vulkanData,
	VkCommandPool commandPool,
	const void * imageData,
	VkDeviceSize imageDataSize,
	VkExtent3D extent,
	VkFormat format,
	VkImageAspectFlags aspectFlag,
	uint32_t layer_count,
	VkImage image) {

	VkCommandBuffer commandBuffer = beginSingleTimeCommand(vulkanData, commandPool);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	VulkanInitialize::createBuffer(vulkanData, imageDataSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingMemory);

	void *mapData;
	vkMapMemory(vulkanData->device, stagingMemory, 0, imageDataSize, 0, (void **)&mapData);
	memcpy(mapData, imageData, static_cast<size_t>(imageDataSize));
	vkUnmapMemory(vulkanData->device, stagingMemory);

	transitionImageLayout(image, format,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT, commandBuffer);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageExtent = extent;
	region.imageOffset = { 0, 0, 0 };
	region.imageSubresource.aspectMask = aspectFlag;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = layer_count;
	region.imageSubresource.mipLevel = 0;

	vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommand(vulkanData, commandPool, commandBuffer);

	vkDestroyBuffer(vulkanData->device, stagingBuffer, nullptr);
	vkFreeMemory(vulkanData->device, stagingMemory, nullptr);
}
