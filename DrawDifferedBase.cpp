#include "DrawDifferedBase.h"


DrawDiffered::DrawDifferedBase::DrawDifferedBase(
	const VulkanEngineApplication::VulkanData * vulkanData,
	const VulkanEngineApplication::VulkanEngineData * vulkanEngineData,
	const std::shared_ptr<Draw::DrawDifferedFramebuffer> &framebuffer,
	const std::shared_ptr<Draw::Model>& model,
	Draw::DrawDataTypes drawType):
	vulkanData(vulkanData), vulkanEngineData(vulkanEngineData),
	model(model), drawType(static_cast<uint32_t>(drawType)),
	framebuffer(framebuffer) {

	initializePipelineDefault();
}

void DrawDiffered::DrawDifferedBase::initializePipelineDefault(void) {
	mRasterizationStateCreateInfo = {};
	mRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	mRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	mRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	mRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;//VK_POLYGON_MODE_LINE VK_POLYGON_MODE_FILL
	mRasterizationStateCreateInfo.lineWidth = 1.0f;
	mRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;//VK_CULL_MODE_BACK_BIT
	mRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	mRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	mRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	mRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	mRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

	mMultisampleStateCreateInfo = {};
	mMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	mMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	mMultisampleStateCreateInfo.minSampleShading = 1.0f;
	mMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	mMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
	mMultisampleStateCreateInfo.pSampleMask = nullptr;
	mMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState mColorBlendAttachmentState = {};
	mColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	mColorBlendAttachmentState.blendEnable = VK_FALSE;
	mColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	mColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	mColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	mColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	mColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	mColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	pipelineColorAttachmentState.resize(framebuffer->getFramebufferColorAttachmentCount());//std::vector<VkPipelineColorBlendAttachmentState>
	for (uint32_t i = 0; i < pipelineColorAttachmentState.size(); ++i) {
		pipelineColorAttachmentState[i] = mColorBlendAttachmentState;
	}

	mColorBlendStateCreateInfo = {};
	mColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	mColorBlendStateCreateInfo.attachmentCount = static_cast<decltype(mColorBlendStateCreateInfo.attachmentCount)>(pipelineColorAttachmentState.size());
	mColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	mColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	mColorBlendStateCreateInfo.pAttachments = pipelineColorAttachmentState.data();
	mColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	mColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	mColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	mColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	depthStancilCreateInfo = {};
	depthStancilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStancilCreateInfo.depthTestEnable = VK_TRUE;
	depthStancilCreateInfo.depthWriteEnable = VK_TRUE;
	depthStancilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStancilCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStancilCreateInfo.minDepthBounds = 0.0f;
	depthStancilCreateInfo.maxDepthBounds = 1.0f;
	depthStancilCreateInfo.stencilTestEnable = VK_FALSE;
	depthStancilCreateInfo.front = {};
	depthStancilCreateInfo.back = {};

	mGraphicsPipelineCreateInfoStruct = {};
	mGraphicsPipelineCreateInfoStruct.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	mGraphicsPipelineCreateInfoStruct.pRasterizationState = &mRasterizationStateCreateInfo;
	mGraphicsPipelineCreateInfoStruct.pMultisampleState = &mMultisampleStateCreateInfo;
	mGraphicsPipelineCreateInfoStruct.pColorBlendState = &mColorBlendStateCreateInfo;
	mGraphicsPipelineCreateInfoStruct.pDepthStencilState = &depthStancilCreateInfo;

	mGraphicsPipelineCreateInfoStruct.pTessellationState = nullptr;
	mGraphicsPipelineCreateInfoStruct.pDynamicState = nullptr;
	mGraphicsPipelineCreateInfoStruct.renderPass = framebuffer->getRenderPass();
	mGraphicsPipelineCreateInfoStruct.subpass = 0;
	mGraphicsPipelineCreateInfoStruct.basePipelineHandle = VK_NULL_HANDLE;
	mGraphicsPipelineCreateInfoStruct.basePipelineIndex = -1;
}

void DrawDiffered::DrawDifferedBase::createGraphicsPipeline() {

	VkViewport mViewport = {};
	mViewport.x = 0.0f;
	mViewport.y = 0.0f;
	mViewport.width = static_cast<float>(framebuffer->getFramebufferSize().width);
	mViewport.height = static_cast<float>(framebuffer->getFramebufferSize().height);
	mViewport.minDepth = 0.0f;
	mViewport.maxDepth = 1.0f;

	VkRect2D mScissorRect = {};
	mScissorRect.extent.width = static_cast<decltype(mScissorRect.extent.width)>(mViewport.width);
	mScissorRect.extent.height = static_cast<decltype(mScissorRect.extent.height)>(mViewport.height);
	mScissorRect.offset = { 0, 0 };

	VkPipelineViewportStateCreateInfo mPipelineViewportStateCreateInfo = {};
	mPipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	mPipelineViewportStateCreateInfo.pScissors = &mScissorRect;
	mPipelineViewportStateCreateInfo.scissorCount = 1;
	mPipelineViewportStateCreateInfo.pViewports = &mViewport;
	mPipelineViewportStateCreateInfo.viewportCount = 1;

	mGraphicsPipelineCreateInfoStruct.pViewportState = &mPipelineViewportStateCreateInfo;

	VkResult result = vkCreateGraphicsPipelines(vulkanData->device, VK_NULL_HANDLE, 1, &mGraphicsPipelineCreateInfoStruct, nullptr, &drawDifferredPipeline);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to create graphics pipeline!");
	}
}
