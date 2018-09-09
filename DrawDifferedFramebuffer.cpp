#include "DrawDifferedFramebuffer.h"


Draw::DrawDifferedFramebuffer::DrawDifferedFramebuffer(
	const VulkanEngineApplication::VulkanData * vulkanData,
	uint32_t width, uint32_t height):
	vulkanData(vulkanData), renderPass(VK_NULL_HANDLE), 
	framebuffer(VK_NULL_HANDLE) {

	//framebufferTextures.resize(4);
	attachmentFormat = { VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, vulkanData->depthFormat };

	createRenderPass();
	createFramebufferTextures(width, height);
	createFramebuffer();
}

Draw::DrawDifferedFramebuffer::~DrawDifferedFramebuffer() {
	vkDestroyRenderPass(vulkanData->device, renderPass, nullptr);
	vkDestroyFramebuffer(vulkanData->device, framebuffer, nullptr);
}

void Draw::DrawDifferedFramebuffer::createRenderPass(void) {
	VkAttachmentDescription depthAttachment = {};
	//depthAttachment.format = attachmentFormat[textureCount - 1];//vulkanData->depthFormat;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;// VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription mAttachmentDescription = {};
	//mAttachmentDescription.format = VK_FORMAT_R8G8B8A8_UNORM;//R32G32B32A32_SFLOAT;
	mAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	mAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	mAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	mAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	mAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	mAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	mAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentReference = {};
	depthAttachmentReference.attachment = textureCount - 1;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::array<VkAttachmentReference, textureCount - 1> mColorAttachmentReference = {};
	for (uint32_t i = 0; i < mColorAttachmentReference.size(); ++i) {
		mColorAttachmentReference[i].attachment = i;
		mColorAttachmentReference[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	std::array<VkSubpassDescription, 1>  mSubpassStruct = {};
	mSubpassStruct[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	mSubpassStruct[0].colorAttachmentCount = static_cast<decltype(mSubpassStruct[0].colorAttachmentCount)>(mColorAttachmentReference.size());
	mSubpassStruct[0].pColorAttachments = mColorAttachmentReference.data();
	mSubpassStruct[0].pDepthStencilAttachment = &depthAttachmentReference;

	std::array<VkAttachmentDescription, textureCount> attachment = { mAttachmentDescription, mAttachmentDescription, mAttachmentDescription , depthAttachment };
	for (uint32_t i = 0; i < textureCount; ++i) {
		attachment[i].format = attachmentFormat[i];
	}

	VkSubpassDependency mDependency = {};
	mDependency.srcSubpass = static_cast<decltype(mDependency.srcSubpass)>(mSubpassStruct.size() - 1);//VK_SUBPASS_EXTERNAL;
	mDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
	mDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;//VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
	mDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	mDependency.dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	mDependency.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	//TODO mDependency

	VkRenderPassCreateInfo mRenderPassCreateInfoStruct = {};
	mRenderPassCreateInfoStruct.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	mRenderPassCreateInfoStruct.attachmentCount = static_cast<uint32_t>(attachment.size());
	mRenderPassCreateInfoStruct.pAttachments = attachment.data();
	mRenderPassCreateInfoStruct.subpassCount = static_cast<decltype(mRenderPassCreateInfoStruct.subpassCount)>(mSubpassStruct.size());
	mRenderPassCreateInfoStruct.pSubpasses = mSubpassStruct.data();
	mRenderPassCreateInfoStruct.dependencyCount = 1;
	mRenderPassCreateInfoStruct.pDependencies = &mDependency;

	if (vkCreateRenderPass(vulkanData->device, &mRenderPassCreateInfoStruct, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to create render pass!");
	}
}

void Draw::DrawDifferedFramebuffer::createFramebufferTextures(uint32_t width, uint32_t height) {
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.arrayLayers = 1;
	imageInfo.flags = 0;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.pNext = nullptr;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	imageInfo.extent = { width, height, 1};
	imageInfo.mipLevels = Texture::getMipCount(imageInfo.extent);
	//imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;//VK_FORMAT_R32G32B32A32_SFLOAT;

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	//viewInfo.format = imageInfo.format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.layerCount = imageInfo.arrayLayers;
	viewInfo.subresourceRange.levelCount = imageInfo.mipLevels;

	for (uint32_t i = 0; i < textureCount - 1; ++i) {
		imageInfo.format = viewInfo.format = attachmentFormat[i];
		framebufferTextures[i] = std::make_unique<Texture>(vulkanData, imageInfo, viewInfo);
	}

	imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.format = viewInfo.format = attachmentFormat[textureCount - 1];
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	framebufferTextures[textureCount - 1] = std::make_unique<Texture>(vulkanData, imageInfo, viewInfo);
}

void Draw::DrawDifferedFramebuffer::createFramebuffer(void) {
	std::array<VkImageView, textureCount> mAttachments = {};
	for (uint32_t i = 0; i < framebufferTextures.size(); ++i) {
		mAttachments[i] = framebufferTextures[i]->getImageData().imageView;
	}
	framebufferSize = framebufferTextures[0]->getImageData().imageExtent;

	VkFramebufferCreateInfo mFramebufferCreateInfoStruct = {};
	mFramebufferCreateInfoStruct.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	mFramebufferCreateInfoStruct.renderPass = renderPass;
	mFramebufferCreateInfoStruct.attachmentCount = static_cast<uint32_t>(mAttachments.size());
	mFramebufferCreateInfoStruct.pAttachments = mAttachments.data();
	mFramebufferCreateInfoStruct.height = framebufferSize.height;
	mFramebufferCreateInfoStruct.width = framebufferSize.width;
	mFramebufferCreateInfoStruct.layers = 1;

	if (vkCreateFramebuffer(vulkanData->device, &mFramebufferCreateInfoStruct, nullptr, &framebuffer) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to create framebuffer!");
	}
}

//void Draw::DrawDifferedFramebuffer::setReading(void) {
//	VkCommandBuffer commandBuffer = VulkanInitialize::beginSingleTimeCommand(vulkanData, vulkanData->commandPool);
//	VkImageAspectFlags aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
//	
//
//	VulkanInitialize::transitionImageLayout(vulkanData->depthBuffer,
//		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
//		0, 0,
//		0, 0,
//		aspect,
//		commandBuffer);
//	VulkanInitialize::endSingleTimeCommand(vulkanData, vulkanData->commandPool, commandBuffer);
//}

uint32_t Draw::DrawDifferedFramebuffer::getFramebufferColorAttachmentCount(void) const {
	assert(textureCount > 0);
	return textureCount - 1;
}

const ImageManager::ImageData &Draw::DrawDifferedFramebuffer::getImageByIndex(uint32_t idx) const {
	assert(idx >= 0 && idx < textureCount);
	return framebufferTextures[idx]->getImageData();// .image;
}

VkRenderPass Draw::DrawDifferedFramebuffer::getRenderPass(void) const {
	return renderPass;
}

VkFramebuffer Draw::DrawDifferedFramebuffer::getFramebuffer(void) const {
	return framebuffer;
}

VkExtent3D Draw::DrawDifferedFramebuffer::getFramebufferSize(void) const {
	return framebufferSize;
}
