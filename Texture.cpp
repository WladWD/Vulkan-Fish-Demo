#include "Texture.h"


Draw::Texture::Texture(
	const VulkanEngineApplication::VulkanData * vulkanData,
	const VkImageCreateInfo & imageInfo, 
	const VkImageViewCreateInfo & viewInfo):
	vulkanData(vulkanData), imageLayout(VK_IMAGE_LAYOUT_UNDEFINED) {

	createImage(imageInfo);
	createImageView(viewInfo);
}

Draw::Texture::~Texture() { 
	vkDestroyImageView(vulkanData->device, imageData.imageView, nullptr);
	vkDestroyImage(vulkanData->device, imageData.image, nullptr);
	vkFreeMemory(vulkanData->device, imageData.imageMemory, nullptr);
}

void Draw::Texture::createImage(const VkImageCreateInfo & imageInfo) {
	imageData.imageExtent = imageInfo.extent;
	imageData.imageFormat = imageInfo.format;
	imageData.layerCount = imageInfo.arrayLayers;
	imageData.levelCount = imageInfo.mipLevels;
	imageLayout = imageInfo.initialLayout;
	
	VulkanInitialize::createImage(vulkanData, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, &imageInfo, imageData.image, imageData.imageMemory);
}

void Draw::Texture::createImageView(const VkImageViewCreateInfo & c_viewInfo) {
	VkImageViewCreateInfo viewInfo = c_viewInfo;
	viewInfo.image = imageData.image;

	imageAspect = viewInfo.subresourceRange.aspectMask;
	if (vkCreateImageView(vulkanData->device, &viewInfo, nullptr, &imageData.imageView) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed create image view");
	}
}

void Draw::Texture::transitionLayout(
	VkImageLayout targetImageLayout,
	VkAccessFlags srcAccess, VkAccessFlags dstAccess,
	VkPipelineStageFlags srcStageMack, VkPipelineStageFlags dstStageMack) {

	VkCommandBuffer commandBuffer = VulkanInitialize::beginSingleTimeCommand(vulkanData, vulkanData->commandPool);

	VkImageMemoryBarrier imageBarrier = {};
	imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageBarrier.oldLayout = imageLayout;
	imageBarrier.newLayout = targetImageLayout;
	imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.image = imageData.image;
	imageBarrier.subresourceRange.aspectMask = imageAspect;
	imageBarrier.subresourceRange.baseArrayLayer = 0;
	imageBarrier.subresourceRange.baseMipLevel = 0;
	imageBarrier.subresourceRange.layerCount = imageData.layerCount;
	imageBarrier.subresourceRange.levelCount = imageData.levelCount;
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

	VulkanInitialize::endSingleTimeCommand(vulkanData, vulkanData->commandPool, commandBuffer);

	targetImageLayout = imageBarrier.newLayout;
}

//void Draw::Texture::generateMipLevels(void) {
//
//	VkCommandBuffer commandBuffer = VulkanInitialize::beginSingleTimeCommand(vulkanData, vulkanData->commandPool);
//	VkImageMemoryBarrier imageMemmoryBarrier = {};
//	imageMemmoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//	imageMemmoryBarrier.pNext = nullptr;
//	imageMemmoryBarrier.image = imageData.image;
//	imageMemmoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//	imageMemmoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//	imageMemmoryBarrier.subresourceRange.aspectMask = imageAspect;
//	imageMemmoryBarrier.subresourceRange.baseArrayLayer = 0;
//	imageMemmoryBarrier.subresourceRange.layerCount = imageData.layerCount;
//	imageMemmoryBarrier.subresourceRange.levelCount = 1;
//
//	int32_t mipWidth = imageData.imageExtent.width;
//	int32_t mipHeight = imageData.imageExtent.height;
//	int32_t mipDepth = imageData.imageExtent.depth;
//
//	for (uint32_t i = 1; i < imageData.levelCount; ++i) {
//
//		imageMemmoryBarrier.subresourceRange.baseMipLevel = i - 1;
//		imageMemmoryBarrier.oldLayout = imageLayout;//VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//		imageMemmoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//
//		imageMemmoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//		imageMemmoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//		vkCmdPipelineBarrier(commandBuffer,
//			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
//			0, nullptr, 0, nullptr,
//			1, &imageMemmoryBarrier);
//
//		VkImageBlit imageBlit = {};
//		imageBlit.srcOffsets[0] = { 0, 0, 0 };
//		imageBlit.srcOffsets[1] = { mipWidth, mipHeight, mipDepth };
//		imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		imageBlit.srcSubresource.baseArrayLayer = 0;
//		imageBlit.srcSubresource.layerCount = imageData.layerCount;
//		imageBlit.srcSubresource.mipLevel = imageMemmoryBarrier.subresourceRange.baseMipLevel;
//
//		imageBlit.dstOffsets[0] = { 0, 0, 0 };
//		imageBlit.dstOffsets[1] = { std::max(1, mipWidth >> 1),  std::max(1, mipHeight >> 1),  std::max(1, mipDepth >> 1) };
//		imageBlit.dstSubresource.aspectMask = imageBlit.srcSubresource.aspectMask;
//		imageBlit.dstSubresource.baseArrayLayer = imageBlit.srcSubresource.baseArrayLayer;
//		imageBlit.dstSubresource.layerCount = imageBlit.srcSubresource.layerCount;
//		imageBlit.dstSubresource.mipLevel = i;
//
//		vkCmdBlitImage(commandBuffer,
//			imageData.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
//			imageData.image, imageMemmoryBarrier.oldLayout,
//			1, &imageBlit,
//			VK_FILTER_LINEAR);
//
//		imageMemmoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//		imageMemmoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//		imageMemmoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//		imageMemmoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//		vkCmdPipelineBarrier(commandBuffer,
//			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
//			0, nullptr,
//			0, nullptr,
//			1, &imageMemmoryBarrier);
//
//		if (mipWidth > 1) mipWidth >>= 1;
//		if (mipHeight > 1) mipHeight >>= 1;
//		if (mipDepth > 1) mipDepth >>= 1;
//	}
//
//	imageMemmoryBarrier.subresourceRange.baseMipLevel = imageData.levelCount - 1;
//	imageMemmoryBarrier.oldLayout = imageLayout;
//	imageMemmoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//	imageMemmoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//	imageMemmoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//	vkCmdPipelineBarrier(commandBuffer,
//		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,//VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
//		0, nullptr,
//		0, nullptr,
//		1, &imageMemmoryBarrier);
//
//	VulkanInitialize::endSingleTimeCommand(vulkanData, vulkanData->commandPool, commandBuffer);
//}

const ImageManager::ImageData & Draw::Texture::getImageData(void) {
	return imageData;
}

uint32_t Draw::Texture::getMipCount(const VkExtent3D & extent) {
	return static_cast<uint32_t>(
		std::floor(
			std::log2(
				std::max(
					extent.depth, std::max(extent.width, extent.height))))) + 1;;
}
