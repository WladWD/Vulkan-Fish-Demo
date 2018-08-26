#include "DrawTextLine.h"

Engine::DrawTextLine::DrawTextLine(const VulkanEngineApplication::VulkanData * vulkanData,
	const VulkanEngineApplication::VulkanEngineData *vulkanEngineData) : vulkanData(vulkanData), vulkanEngineData(vulkanEngineData) {
}

Engine::DrawTextLine::~DrawTextLine() {
	delete shaderPipeline;

	vkDestroyImageView(vulkanData->device, sourceImageView, nullptr);
	vkDestroyImage(vulkanData->device, sourceImage, nullptr);
	vkFreeMemory(vulkanData->device, imageMemory, nullptr);

	vkDestroyPipeline(vulkanData->device, drawImagePipeline, nullptr);
}

void Engine::DrawTextLine::resize(void) {
	vkDestroyPipeline(vulkanData->device, drawImagePipeline, nullptr);
	initializePipeline();
}

void Engine::DrawTextLine::initialize(const Asset::AssetLoader *assetLoader) {
	loadImage("Resources\\Images\\font.dds", assetLoader);
	shaderPipeline = new Shader::DrawTextLineShaderPipeline(assetLoader, vulkanData, { 16, 16 });

	updateDescriptorSet();
	initializePipeline();
}

void Engine::DrawTextLine::loadImage(const char * imageName, const Asset::AssetLoader *assetLoader) {
	uint8_t *data = nullptr;
	int64_t dataSize = 0;
	assetLoader->loadAssetByName(std::string(imageName), &data, dataSize);

	gli::texture tex = gli::load_dds((char *)data, static_cast<size_t>(dataSize));

	VkExtent3D extent = { static_cast<uint32_t>(tex.extent().x), static_cast<uint32_t>(tex.extent().y), static_cast<uint32_t>(tex.extent().z) };
	auto f = tex.format();
	VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
	createImage(static_cast<const char *>(tex.data()), static_cast<uint32_t>(tex.size()), extent, format);

	delete[] data;
}

void Engine::DrawTextLine::createImage(const char * data, uint32_t size, VkExtent3D extent, VkFormat format) {

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.arrayLayers = 1;
	imageInfo.flags = 0;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.mipLevels = 1;
	imageInfo.pNext = nullptr;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	imageInfo.extent = extent;
	imageInfo.format = format;

	this->sourceExtent = extent;

	VulkanInitialize::createImage(vulkanData, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, &imageInfo, sourceImage, imageMemory);
	VulkanInitialize::initializeImage(vulkanData, vulkanData->commandPool, data, size, extent, format, VK_IMAGE_ASPECT_COLOR_BIT, 1, sourceImage);

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = sourceImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.layerCount = 1;
	viewInfo.subresourceRange.levelCount = 1;

	if (vkCreateImageView(vulkanData->device, &viewInfo, nullptr, &sourceImageView) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed create image view");
	}
}

void Engine::DrawTextLine::initializePipeline(void) {

	auto mVertexAttributeDescription = VertexFormat::VertexPosition::getAttributeDescription();
	auto mVertexBindingDescrioption = VertexFormat::VertexPosition::getBindingDescription();

	VkPipelineVertexInputStateCreateInfo mVertexInputStageCreateInfo = {};
	mVertexInputStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	mVertexInputStageCreateInfo.pVertexAttributeDescriptions = mVertexAttributeDescription.data();
	mVertexInputStageCreateInfo.pVertexBindingDescriptions = &mVertexBindingDescrioption;
	mVertexInputStageCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(mVertexAttributeDescription.size());
	mVertexInputStageCreateInfo.vertexBindingDescriptionCount = 1U;

	VkPipelineInputAssemblyStateCreateInfo mInputAssemblyStateCreateInfo = {};
	mInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	mInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
	mInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkViewport mViewport = {};
	mViewport.x = 0.0f;
	mViewport.y = 0.0f;
	mViewport.width = static_cast<float>(vulkanData->mSwapChainImageExtent.width);
	mViewport.height = static_cast<float>(vulkanData->mSwapChainImageExtent.height);
	mViewport.minDepth = 0.0f;
	mViewport.maxDepth = 1.0f;

	VkRect2D mScissorRect = {};
	mScissorRect.extent = vulkanData->mSwapChainImageExtent;
	mScissorRect.offset = { 0, 0 };

	VkPipelineViewportStateCreateInfo mPipelineViewportStateCreateInfo = {};
	mPipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	mPipelineViewportStateCreateInfo.pScissors = &mScissorRect;
	mPipelineViewportStateCreateInfo.scissorCount = 1;
	mPipelineViewportStateCreateInfo.pViewports = &mViewport;
	mPipelineViewportStateCreateInfo.viewportCount = 1;

	VkPipelineRasterizationStateCreateInfo mRasterizationStateCreateInfo = {};
	mRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	mRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	mRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	mRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	mRasterizationStateCreateInfo.lineWidth = 1.0f;
	mRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;//VK_CULL_MODE_BACK_BIT
	mRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	mRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	mRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	mRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	mRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo mMultisampleStateCreateInfo = {};
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
	mColorBlendAttachmentState.blendEnable = VK_TRUE;
	mColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	mColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	mColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	mColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	mColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	mColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo mColorBlendStateCreateInfo = {};
	mColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	mColorBlendStateCreateInfo.attachmentCount = 1;
	mColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	mColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	mColorBlendStateCreateInfo.pAttachments = &mColorBlendAttachmentState;
	mColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	mColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	mColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	mColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	VkPipelineDepthStencilStateCreateInfo depthStancilCreateInfo = {};
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

	VkGraphicsPipelineCreateInfo mGraphicsPipelineCreateInfoStruct = {};
	mGraphicsPipelineCreateInfoStruct.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	mGraphicsPipelineCreateInfoStruct.stageCount = static_cast<uint32_t>(shaderPipeline->getShaderStagesCount());
	mGraphicsPipelineCreateInfoStruct.pStages = shaderPipeline->getShaderStages();
	mGraphicsPipelineCreateInfoStruct.pVertexInputState = &mVertexInputStageCreateInfo;
	mGraphicsPipelineCreateInfoStruct.pInputAssemblyState = &mInputAssemblyStateCreateInfo;
	mGraphicsPipelineCreateInfoStruct.pViewportState = &mPipelineViewportStateCreateInfo;
	mGraphicsPipelineCreateInfoStruct.pRasterizationState = &mRasterizationStateCreateInfo;
	mGraphicsPipelineCreateInfoStruct.pMultisampleState = &mMultisampleStateCreateInfo;
	mGraphicsPipelineCreateInfoStruct.pDepthStencilState = nullptr;
	mGraphicsPipelineCreateInfoStruct.pColorBlendState = &mColorBlendStateCreateInfo;
	mGraphicsPipelineCreateInfoStruct.pDynamicState = nullptr;
	mGraphicsPipelineCreateInfoStruct.layout = shaderPipeline->getPipelineLayout();
	mGraphicsPipelineCreateInfoStruct.renderPass = vulkanData->swapchainRenderpass;
	mGraphicsPipelineCreateInfoStruct.pDepthStencilState = &depthStancilCreateInfo;
	mGraphicsPipelineCreateInfoStruct.subpass = 0;
	mGraphicsPipelineCreateInfoStruct.basePipelineHandle = VK_NULL_HANDLE;
	mGraphicsPipelineCreateInfoStruct.basePipelineIndex = -1;

	VkResult result = vkCreateGraphicsPipelines(vulkanData->device, VK_NULL_HANDLE, 1, &mGraphicsPipelineCreateInfoStruct, nullptr, &drawImagePipeline);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to create graphics pipeline!");
	}
}

void Engine::DrawTextLine::updateDescriptorSet(void) {
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = sourceImageView;
	imageInfo.sampler = vulkanEngineData->samplers->getMinMaxMag_Linear_UVW_Wrap(0, 0);

	std::array<VkWriteDescriptorSet, 1> writeDescriptors = {};
	writeDescriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptors[0].dstSet = shaderPipeline->getDescriptorSets()[0];
	writeDescriptors[0].dstBinding = 0;
	writeDescriptors[0].dstArrayElement = 0;
	writeDescriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptors[0].descriptorCount = 1;
	writeDescriptors[0].pBufferInfo = nullptr;
	writeDescriptors[0].pImageInfo = &imageInfo;
	writeDescriptors[0].pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(vulkanData->device, static_cast<uint32_t>(writeDescriptors.size()), writeDescriptors.data(), 0, nullptr);
}

void Engine::DrawTextLine::setMessage(std::string text,
	glm::vec2 position,
	float scale,
	DrawTextAligment aligment,
	DrawTextBindingPoint bindingPoint) {
	pushBuffer.data.cData[0] = glm::min(static_cast<uint32_t>(text.length()), 112U);
	memcpy(pushBuffer.data.cData + 1, text.c_str(), static_cast<size_t>(pushBuffer.data.cData[0]));
	
	float ar = static_cast<float>(vulkanData->mSwapChainImageExtent.width) / vulkanData->mSwapChainImageExtent.height;
	position = position * 2.0f - glm::vec2(1.0);

	glm::vec2 textScale = (aligment == DrawTextAligment::AligmentWidth) ? glm::vec2(scale, scale / text.length() * ar) : textScale = glm::vec2(scale * text.length() / ar, scale);
	glm::vec2 offset = glm::vec2(0.0);
	switch (bindingPoint) {
	case DrawTextBindingPoint::LeftTop: offset = glm::vec2(textScale.x, textScale.y); break;
	case DrawTextBindingPoint::LeftBotton: offset = glm::vec2(textScale.x, -textScale.y); break;
	case DrawTextBindingPoint::RightTop: offset = glm::vec2(-textScale.x, textScale.y); break;
	case DrawTextBindingPoint::RightBotton: offset = glm::vec2(-textScale.x, -textScale.y); break;
	}
	//glm::vec2 offset = (bindingPoint == DrawTextBindingPoint::LeftTop) ? glm::vec2(textScale.x, textScale.y) : glm::vec2(textScale.x, -textScale.y);
	pushBuffer.scaleOffset = glm::vec4(textScale.x, textScale.y, position.x + offset.x, position.y + offset.y);
}

void Engine::DrawTextLine::draw(VkCommandBuffer commandBuffer) {
	
	VkBuffer mBuffers[] = { vulkanEngineData->quadBuffer->getVertexBuffer() };
	VkDeviceSize mOffsets[] = { 0 };

	vkCmdPushConstants(commandBuffer, shaderPipeline->getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pushBuffer), &pushBuffer);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, drawImagePipeline);
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, mBuffers, mOffsets);
	vkCmdBindIndexBuffer(commandBuffer, vulkanEngineData->quadBuffer->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shaderPipeline->getPipelineLayout(), 0, 1, shaderPipeline->getDescriptorSets(), 0, nullptr);
	vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
}


