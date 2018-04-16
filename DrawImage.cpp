#include "DrawImage.h"


Engine::DrawImage::DrawImage(const VulkanEngineApplication::VulkanData * vulkanData) : vulkanData(vulkanData) {
}

Engine::DrawImage::~DrawImage() {
}

void Engine::DrawImage::initialize(void) {
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

	VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

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


