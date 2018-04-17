#include "VulkanFramebuffer.h"


VulkanEngineApplication::VulkanFramebuffer::VulkanFramebuffer(const VulkanEngineApplication::VulkanData * vulkanData) : vulkanData(const_cast<VulkanEngineApplication::VulkanData *>(vulkanData)) {
}

VulkanEngineApplication::VulkanFramebuffer::~VulkanFramebuffer() {
	free();
}

void VulkanEngineApplication::VulkanFramebuffer::createRenderPass(void) {
	/*VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;*/

	VkAttachmentDescription mAttachmentDescription = {};
	mAttachmentDescription.format = vulkanData->mSwapChainImageFormat;
	mAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	mAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	mAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	mAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	mAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	mAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	mAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	//VkAttachmentReference depthAttachmentReference = {};
	//depthAttachmentReference.attachment = 1;
	//depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference mColorAttachmentReference = {};
	mColorAttachmentReference.attachment = 0;
	mColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription mSubpassStruct = {};
	mSubpassStruct.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	mSubpassStruct.colorAttachmentCount = 1;
	mSubpassStruct.pColorAttachments = &mColorAttachmentReference;
	mSubpassStruct.pDepthStencilAttachment = nullptr;//&depthAttachmentReference;

	std::array<VkAttachmentDescription, 1> attachment = { mAttachmentDescription };// , depthAttachment };

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

	if (vkCreateRenderPass(vulkanData->device, &mRenderPassCreateInfoStruct, nullptr, &vulkanData->swapchainRenderpass) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to create render pass!");
	}
}

void VulkanEngineApplication::VulkanFramebuffer::createFramebuffer(void) {
	vulkanData->swapchainFramebuffer.resize(vulkanData->swapchainImage.size());

	for (size_t i = 0; i < vulkanData->swapchainFramebuffer.size(); ++i) {
		std::array<VkImageView, 1> mAttachments = {
			vulkanData->swapchainImageView[i]
			//depthImageView
		};

		VkFramebufferCreateInfo mFramebufferCreateInfoStruct = {};
		mFramebufferCreateInfoStruct.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		mFramebufferCreateInfoStruct.renderPass = vulkanData->swapchainRenderpass;
		mFramebufferCreateInfoStruct.attachmentCount = static_cast<uint32_t>(mAttachments.size());
		mFramebufferCreateInfoStruct.pAttachments = mAttachments.data();
		mFramebufferCreateInfoStruct.height = vulkanData->mSwapChainImageExtent.height;
		mFramebufferCreateInfoStruct.width = vulkanData->mSwapChainImageExtent.width;
		mFramebufferCreateInfoStruct.layers = 1;

		if (vkCreateFramebuffer(vulkanData->device, &mFramebufferCreateInfoStruct, nullptr, &vulkanData->swapchainFramebuffer[i]) != VK_SUCCESS) {
			throw std::runtime_error("[DBG]\tFailed to create framebuffer!");
		}
	}
}

void VulkanEngineApplication::VulkanFramebuffer::initialize(void) {
	createRenderPass();
	createFramebuffer();
}

void VulkanEngineApplication::VulkanFramebuffer::free(void) {
	for (size_t i = 0; i < vulkanData->swapchainFramebuffer.size(); ++i)
		vkDestroyFramebuffer(vulkanData->device, vulkanData->swapchainFramebuffer[i], nullptr);

	vkDestroyRenderPass(vulkanData->device, vulkanData->swapchainRenderpass, nullptr);
}
