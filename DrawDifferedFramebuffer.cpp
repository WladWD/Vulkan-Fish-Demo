#include "DrawDifferedFramebuffer.h"


Draw::DrawDifferedFramebuffer::DrawDifferedFramebuffer(
	const VulkanEngineApplication::VulkanData * vulkanData,
	uint32_t width, uint32_t height):
	vulkanData(vulkanData), renderPass(VK_NULL_HANDLE), 
	framebuffer(VK_NULL_HANDLE) {

	framebufferTextures.resize(4);
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
	depthAttachment.format = vulkanData->depthFormat;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription mAttachmentDescription = {};
	mAttachmentDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	mAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	mAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	mAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	mAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	mAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	mAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	mAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentReference = {};
	depthAttachmentReference.attachment = 3;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::array<VkAttachmentReference, 3> mColorAttachmentReference = {};
	for (uint32_t i = 0; i < mColorAttachmentReference.size(); ++i) {
		mColorAttachmentReference[i].attachment = i;
		mColorAttachmentReference[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	VkSubpassDescription mSubpassStruct = {};
	mSubpassStruct.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	mSubpassStruct.colorAttachmentCount = static_cast<decltype(mSubpassStruct.colorAttachmentCount)>(mColorAttachmentReference.size());
	mSubpassStruct.pColorAttachments = mColorAttachmentReference.data();
	mSubpassStruct.pDepthStencilAttachment = &depthAttachmentReference;

	std::array<VkAttachmentDescription, 4> attachment = { mAttachmentDescription, mAttachmentDescription, mAttachmentDescription, depthAttachment };

	VkRenderPassCreateInfo mRenderPassCreateInfoStruct = {};
	mRenderPassCreateInfoStruct.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	mRenderPassCreateInfoStruct.attachmentCount = static_cast<uint32_t>(attachment.size());
	mRenderPassCreateInfoStruct.pAttachments = attachment.data();
	mRenderPassCreateInfoStruct.subpassCount = 1;
	mRenderPassCreateInfoStruct.pSubpasses = &mSubpassStruct;

	VkSubpassDependency mDependency = {};
	mDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	mDependency.dstSubpass = 0;
	mDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;//VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
	mDependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	mDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	mDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

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
	imageInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = imageInfo.format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.layerCount = imageInfo.arrayLayers;
	viewInfo.subresourceRange.levelCount = imageInfo.mipLevels;

	for (uint32_t i = 0; i < framebufferTextures.size() - 1; ++i) {
		framebufferTextures[i] = std::make_unique<Texture>(vulkanData, imageInfo, viewInfo);
	}

	imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.format = vulkanData->depthFormat;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	framebufferTextures[3] = std::make_unique<Texture>(vulkanData, imageInfo, viewInfo);
}

void Draw::DrawDifferedFramebuffer::createFramebuffer(void) {

	std::array<VkImageView, 4> mAttachments = {};
	for (uint32_t i = 0; i < framebufferTextures.size(); ++i) {
		mAttachments[i] = framebufferTextures[i]->getImageData().imageView;
	}

	const auto &imageExtent = framebufferTextures[0]->getImageData().imageExtent;

	VkFramebufferCreateInfo mFramebufferCreateInfoStruct = {};
	mFramebufferCreateInfoStruct.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	mFramebufferCreateInfoStruct.renderPass = vulkanData->swapchainRenderpass;
	mFramebufferCreateInfoStruct.attachmentCount = static_cast<uint32_t>(mAttachments.size());
	mFramebufferCreateInfoStruct.pAttachments = mAttachments.data();
	mFramebufferCreateInfoStruct.height = imageExtent.height;
	mFramebufferCreateInfoStruct.width = imageExtent.width;
	mFramebufferCreateInfoStruct.layers = 1;

	if (vkCreateFramebuffer(vulkanData->device, &mFramebufferCreateInfoStruct, nullptr, &framebuffer) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to create framebuffer!");
	}
}

VkRenderPass Draw::DrawDifferedFramebuffer::getRenderPass(void) const {
	return renderPass;
}

VkFramebuffer Draw::DrawDifferedFramebuffer::getFramebuffer(void) const {
	return framebuffer;
}
