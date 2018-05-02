#include "DrawImage.h"



Engine::DrawImage::DrawImage(const VulkanEngineApplication::VulkanData * vulkanData) : vulkanData(vulkanData) {
}

Engine::DrawImage::~DrawImage() {
}

void Engine::DrawImage::initialize(VkCommandBuffer initializeCommandBuffer, const Asset::AssetLoader *assetLoader) {
	loadImage(initializeCommandBuffer, "Resources\\Images\\initialize.dds", assetLoader);
}

void Engine::DrawImage::loadImage(VkCommandBuffer commandBuffer, const char * imageName, const Asset::AssetLoader *assetLoader) {
	uint8_t *data = nullptr;
	int64_t dataSize = 0;
	assetLoader->loadAssetByName(std::string(imageName), &data, dataSize);

	gli::texture tex = gli::load_dds((char *)data, static_cast<size_t>(dataSize));//gli::load_dds(imageName);
	
	VkExtent3D extent = { static_cast<uint32_t>(tex.extent().x), static_cast<uint32_t>(tex.extent().y), static_cast<uint32_t>(tex.extent().z) };
	auto f = tex.format();
	VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
	createImage(commandBuffer, static_cast<const char *>(tex.data()), tex.size(), extent, format);

	delete[] data;
}

uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if ((typeFilter & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties))
			return i;
	}

	throw std::runtime_error("Failed to find suitable memory type!");
	return -1;
}

void transitionImageLayout(VkImage image, VkFormat format,
	VkImageLayout oldLayout, VkImageLayout newLayout,
	VkAccessFlags srcAccess, VkAccessFlags dstAccess,
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

	VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_HOST_BIT;
	VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

	imageBarrier.srcAccessMask = srcAccess;//VK_ACCESS_MEMORY_READ_BIT;
	imageBarrier.dstAccessMask = dstAccess;// VK_ACCESS_TRANSFER_WRITE_BIT;

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage,
		destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageBarrier);

}

void Engine::DrawImage::createImage(VkCommandBuffer commandBuffer, const char * data, uint32_t size, VkExtent3D extent, VkFormat format) {

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.arrayLayers = 1;
	imageInfo.flags = 0;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageInfo.mipLevels = 1;
	imageInfo.pNext = nullptr;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	imageInfo.extent = extent;
	imageInfo.format = format;

	this->sourceExtent = extent;

	VkResult result;
	if ((result = vkCreateImage(vulkanData->device, &imageInfo, nullptr, &sourceImage)) != VK_SUCCESS) {
		std::runtime_error("[DBG] Failed to create texture");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(vulkanData->device, sourceImage, &memoryRequirements);

	VkMemoryAllocateInfo allocationInfo = {};
	allocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocationInfo.allocationSize = memoryRequirements.size;
	allocationInfo.memoryTypeIndex = findMemoryType(vulkanData->physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	allocationInfo.pNext = nullptr;

	if (vkAllocateMemory(vulkanData->device, &allocationInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to create texture");
	}

	char *mapData = nullptr;
	vkMapMemory(vulkanData->device, imageMemory, 0, size, 0, (void **)&mapData);
	memcpy(mapData, data, size);
	vkUnmapMemory(vulkanData->device, imageMemory);

	vkBindImageMemory(vulkanData->device, sourceImage, imageMemory, 0);

	transitionImageLayout(sourceImage, imageInfo.format,
		VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
		commandBuffer);
}

void Engine::DrawImage::draw(VkCommandBuffer commandBuffer, int32_t frameIdx) {

	VkRenderPassBeginInfo mRenderPassBeginInfo = {};
	mRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	mRenderPassBeginInfo.renderPass = vulkanData->swapchainRenderpass;
	mRenderPassBeginInfo.framebuffer = vulkanData->swapchainFramebuffer[frameIdx];
	mRenderPassBeginInfo.renderArea.offset = { 0, 0 };
	mRenderPassBeginInfo.renderArea.extent.width = vulkanData->mSwapChainImageExtent.width;
	mRenderPassBeginInfo.renderArea.extent.height = vulkanData->mSwapChainImageExtent.height;

	std::array<VkClearValue, 1> mClearValue;
	mClearValue[0].color = { 1.0f, 1.0f, 0.0f, 1.0f };
	//mClearValue[1].depthStencil = { 1.0f, 0 };

	mRenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(mClearValue.size());
	mRenderPassBeginInfo.pClearValues = mClearValue.data();

	vkCmdBeginRenderPass(commandBuffer, &mRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdEndRenderPass(commandBuffer);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkImageBlit blitData = {};

	blitData.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitData.srcSubresource.baseArrayLayer = 0;
	blitData.srcSubresource.layerCount = 1;
	blitData.srcSubresource.mipLevel = 0;

	blitData.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitData.dstSubresource.baseArrayLayer = 0;
	blitData.dstSubresource.layerCount = 1;
	blitData.dstSubresource.mipLevel = 0;

	blitData.srcOffsets[1] = { (int32_t)this->sourceExtent.width, (int32_t)this->sourceExtent.height, 1 };
	blitData.dstOffsets[1] = { (int32_t)vulkanData->mSwapChainImageExtent.width, (int32_t)vulkanData->mSwapChainImageExtent.height, 1 };

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	transitionImageLayout(vulkanData->swapchainImage[frameIdx], vulkanData->mSwapChainImageFormat,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_WRITE_BIT,
		commandBuffer);

	vkCmdBlitImage(commandBuffer,
		sourceImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		vulkanData->swapchainImage[frameIdx], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitData, VK_FILTER_LINEAR);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	transitionImageLayout(vulkanData->swapchainImage[frameIdx], vulkanData->mSwapChainImageFormat,
//		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
//		VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
//		commandBuffer);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	//------------------------------------------------------------------------------------
	//std::array<VkClearColorValue, 1> mClearValue;
	//mClearValue[0].float32[0] = 1.0f;
	//mClearValue[0].float32[1] = 1.0f;
	//mClearValue[0].float32[2] = 0.0f;
	//mClearValue[0].float32[3] = 1.0f;

	//VkImageSubresourceRange imageRange = {};
	//imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//imageRange.levelCount = 1;
	//imageRange.layerCount = 1;

	//transitionImageLayout(vulkanData->swapchainImage[frameIdx], vulkanData->mSwapChainImageFormat,
	//	VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
	//	VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_WRITE_BIT,
	//	commandBuffer);

	//vkCmdClearColorImage(commandBuffer, vulkanData->swapchainImage[frameIdx], 
	//	VK_IMAGE_LAYOUT_GENERAL, mClearValue.data(), 1, &imageRange);

	//transitionImageLayout(vulkanData->swapchainImage[frameIdx], vulkanData->mSwapChainImageFormat,
	//	VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	//	VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
	//	commandBuffer);
}


