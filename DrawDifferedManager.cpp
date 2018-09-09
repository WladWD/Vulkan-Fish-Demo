#include "DrawDifferedManager.h"

DrawDiffered::DrawDifferedManager::DrawDifferedManager(
	const VulkanEngineApplication::VulkanData * vulkanData,
	const VulkanEngineApplication::VulkanEngineData * vulkanEngineData, 
	const std::shared_ptr<Draw::Model>& model, 
	const Asset::AssetLoader * assetLoader): model(model), vulkanData(vulkanData) {

	framebuffer = std::make_shared<Draw::DrawDifferedFramebuffer>(vulkanData, vulkanData->mSwapChainImageExtent.width, vulkanData->mSwapChainImageExtent.height);
	drawDiffered[Draw::DrawDataTypes::DrawDataDiffuse] = std::make_unique<DrawDifferedDiffuse>(vulkanData, vulkanEngineData, model, assetLoader, framebuffer);
}

const std::shared_ptr<Draw::DrawDifferedFramebuffer>& DrawDiffered::DrawDifferedManager::getFramebuffer(void) const {
	return framebuffer;
}

void DrawDiffered::DrawDifferedManager::draw(VkCommandBuffer commandBuffer) {
	std::array<VkClearValue, 4> mClearValue;
	mClearValue[0].color = { 1.0f, 0.0f, 1.0f, 1.0f };
	mClearValue[1].color = { 0.0f, 1.0f, 1.0f, 1.0f };
	mClearValue[2].color = { 1.0f, 1.0f, 0.0f, 1.0f };
	mClearValue[3].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo mRenderPassBeginInfo = {};
	mRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	mRenderPassBeginInfo.renderPass = framebuffer->getRenderPass();
	mRenderPassBeginInfo.framebuffer = framebuffer->getFramebuffer();
	mRenderPassBeginInfo.renderArea.offset = { 0, 0 };
	mRenderPassBeginInfo.renderArea.extent.width = framebuffer->getFramebufferSize().width;
	mRenderPassBeginInfo.renderArea.extent.height = framebuffer->getFramebufferSize().height;
	mRenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(mClearValue.size());
	mRenderPassBeginInfo.pClearValues = mClearValue.data();

	vkCmdBeginRenderPass(commandBuffer, &mRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkBuffer mBuffers[] = { model->buffer->vertexBuffer };//getVertexBuffer
	VkDeviceSize mOffsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, mBuffers, mOffsets);
	vkCmdBindIndexBuffer(commandBuffer, model->buffer->indexBuffer, 0, VK_INDEX_TYPE_UINT32);//getIndexBuffer

	for (const auto &drawDiff: drawDiffered) {
		drawDiff.second->draw(commandBuffer);
	}

	vkCmdEndRenderPass(commandBuffer);



	//VkImageBlit imageBlit = {};
	//imageBlit.srcOffsets[0] = { 0, 0, 0 };
	//imageBlit.srcOffsets[1] = { (int32_t)framebuffer->getFramebufferSize().width,  (int32_t)framebuffer->getFramebufferSize().height, 1 };
	//imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//imageBlit.srcSubresource.baseArrayLayer = 0;
	//imageBlit.srcSubresource.layerCount = 1;
	//imageBlit.srcSubresource.mipLevel = 0;

	//imageBlit.dstOffsets[0] = { 0, 0, 0 };
	//imageBlit.dstOffsets[1] = imageBlit.srcOffsets[1];
	//imageBlit.dstSubresource.aspectMask = imageBlit.srcSubresource.aspectMask;
	//imageBlit.dstSubresource.baseArrayLayer = imageBlit.srcSubresource.baseArrayLayer;
	//imageBlit.dstSubresource.layerCount = imageBlit.srcSubresource.layerCount;
	//imageBlit.dstSubresource.mipLevel = 0;

	//vkCmdBlitImage(vulkanData->commandBuffer[vulkanData->mImageIndex],
	//	framebuffer->getImageByIndex(1), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
	//	vulkanData->swapchainImage[vulkanData->mImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	//	1, &imageBlit,
	//	VK_FILTER_LINEAR);
}
