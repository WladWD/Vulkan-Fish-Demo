#include "DrawImage.h"



Engine::DrawImage::DrawImage(const VulkanEngineApplication::VulkanData * vulkanData) : vulkanData(vulkanData) {
}

Engine::DrawImage::~DrawImage() {
	delete shaderPipeline;
}

void Engine::DrawImage::initialize(VkCommandBuffer initializeCommandBuffer, const Asset::AssetLoader *assetLoader) {
	loadImage(initializeCommandBuffer, "Resources\\Images\\initialize.dds", assetLoader);
	shaderPipeline = new Shader::DrawImageShaderPipeline(assetLoader, vulkanData);

	initializeVertexBuffer(initializeCommandBuffer);
	initializeIndexBuffer(initializeCommandBuffer);
	initializeUniformBuffer();
	initializeSampler();
	updateDescriptorSet();
	initializePipeline();
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

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		imageBarrier.srcAccessMask = 0;
		imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}

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
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.mipLevels = 1;
	imageInfo.pNext = nullptr;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;//VK_IMAGE_USAGE_TRANSFER_SRC_BIT

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
	allocationInfo.memoryTypeIndex = findMemoryType(vulkanData->physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
	allocationInfo.pNext = nullptr;

	if (vkAllocateMemory(vulkanData->device, &allocationInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to create texture");
	}

	//char *mapData = nullptr;
	//vkMapMemory(vulkanData->device, imageMemory, 0, size, 0, (void **)&mapData);
	//memcpy(mapData, data, size);
	//vkUnmapMemory(vulkanData->device, imageMemory);

	vkBindImageMemory(vulkanData->device, sourceImage, imageMemory, 0);


	VkDeviceSize textureSize = size;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;

	createBuffer(textureSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingMemory);

	void *mapData;
	vkMapMemory(vulkanData->device, stagingMemory, 0, textureSize, 0, (void **)&mapData);
	memcpy(mapData, data, textureSize);
	vkUnmapMemory(vulkanData->device, stagingMemory);

	transitionImageLayout(sourceImage, imageInfo.format,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
		0, VK_ACCESS_TRANSFER_WRITE_BIT, commandBuffer);

	//VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	//copyBufferToImage(stagingBuffer, sourceImage, textureSource.extent().x, textureSource.extent().y);
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageExtent = extent;
	region.imageOffset = { 0, 0, 0 };
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageSubresource.mipLevel = 0;

	vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, sourceImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	transitionImageLayout(sourceImage, imageInfo.format,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, commandBuffer);

	
	//----vkDestroyBuffer(vulkanData->device, stagingBuffer, nullptr);
	//----vkFreeMemory(vulkanData->device, stagingMemory, nullptr);

	/*transitionImageLayout(sourceImage, imageInfo.format,
		VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
		commandBuffer);*/

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

void Engine::DrawImage::initializePipeline(void) {

	auto mVertexAttributeDescription = Shader::DrawImageVertex::getAttributeDescription();
	auto mVertexBindingDescrioption = Shader::DrawImageVertex::getBindingDescription();

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
	mColorBlendAttachmentState.blendEnable = VK_FALSE;
	mColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	mColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	mColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
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
	mGraphicsPipelineCreateInfoStruct.stageCount = shaderPipeline->getShaderStagesCount();
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

void Engine::DrawImage::initializeVertexBuffer(VkCommandBuffer commandBuffer) {
	VkDeviceSize bufferSize = sizeof(Shader::DrawImageVertex) * mInputVertex.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemmory;
	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingMemmory);

	void *stagingBufferData;
	vkMapMemory(vulkanData->device, stagingMemmory, 0, bufferSize, 0, &stagingBufferData);
	memcpy(stagingBufferData, mInputVertex.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(vulkanData->device, stagingMemmory);

	createBuffer(bufferSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer, vertexBufferMemory);

	VkBufferCopy copyRegion = {};
	copyRegion.dstOffset = 0;
	copyRegion.srcOffset = 0;
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(commandBuffer, stagingBuffer, vertexBuffer, 1, &copyRegion);

	//vkDestroyBuffer(vulkanData->device, stagingBuffer, nullptr);
	//vkFreeMemory(vulkanData->device, stagingMemmory, nullptr);
}

void Engine::DrawImage::initializeIndexBuffer(VkCommandBuffer commandBuffer) {
	VkDeviceSize bufferSize = sizeof(uint16_t) * inputIndex.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemmory;
	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingMemmory);

	void *stagingBufferData;
	vkMapMemory(vulkanData->device, stagingMemmory, 0, bufferSize, 0, &stagingBufferData);
	memcpy(stagingBufferData, inputIndex.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(vulkanData->device, stagingMemmory);

	createBuffer(bufferSize,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		indexBuffer, indexBufferMemory);

	VkBufferCopy copyRegion = {};
	copyRegion.dstOffset = 0;
	copyRegion.srcOffset = 0;
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(commandBuffer, stagingBuffer, indexBuffer, 1, &copyRegion);

	//vkDestroyBuffer(vulkanData->device, stagingBuffer, nullptr);
	//vkFreeMemory(vulkanData->device, stagingMemmory, nullptr);
}

void Engine::DrawImage::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory& bufferMemory)
{
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

void Engine::DrawImage::initializeUniformBuffer(void) {
	VkDeviceSize uniformBufferSize = sizeof(Shader::DrawImageShaderPipeline::UniformBuffer);
	createBuffer(uniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		uniformBuffer, uniformBufferMemory);
}

void Engine::DrawImage::initializeSampler(void) {
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 1;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0;
	samplerInfo.minLod = 0.0;
	samplerInfo.maxLod = 0.0;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	if (vkCreateSampler(vulkanData->device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed create sampler");
	}
}

void Engine::DrawImage::updateDescriptorSet(void) {
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = sourceImageView;
	imageInfo.sampler = sampler;

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(Shader::DrawImageShaderPipeline::UniformBuffer);

	std::array<VkWriteDescriptorSet, 2> writeDescriptors = {};
	writeDescriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptors[0].dstSet = shaderPipeline->getDescriptorSets()[0];
	writeDescriptors[0].dstBinding = 0;
	writeDescriptors[0].dstArrayElement = 0;
	writeDescriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptors[0].descriptorCount = 1;
	writeDescriptors[0].pBufferInfo = &bufferInfo;
	writeDescriptors[0].pImageInfo = nullptr;
	writeDescriptors[0].pTexelBufferView = nullptr;

	writeDescriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptors[1].dstSet = shaderPipeline->getDescriptorSets()[0];
	writeDescriptors[1].dstBinding = 1;
	writeDescriptors[1].dstArrayElement = 0;
	writeDescriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptors[1].descriptorCount = 1;
	writeDescriptors[1].pBufferInfo = nullptr;
	writeDescriptors[1].pImageInfo = &imageInfo;
	writeDescriptors[1].pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(vulkanData->device, static_cast<uint32_t>(writeDescriptors.size()), writeDescriptors.data(), 0, nullptr);
}

void Engine::DrawImage::updateUniforms(void)
{
	//Update UBO;

	static auto begTime = std::chrono::high_resolution_clock::now();
	auto curTime = std::chrono::high_resolution_clock::now();
	auto deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(curTime - begTime).count();

	buffer.projView = glm::perspective(1.0f, static_cast<float>(vulkanData->mSwapChainImageExtent.width) / static_cast<float>(vulkanData->mSwapChainImageExtent.height), 1.0f, 1000.0f);
	buffer.projView *= glm::lookAt(glm::vec3(0.0, 0.0, 2.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	buffer.projView *= glm::rotate(glm::mat4(1.0), deltaTime, glm::vec3(0.0, 1.0, 0.0));
	//ubo.projView = glm::mat4(1.0);


	Shader::DrawImageShaderPipeline::UniformBuffer *uniformMemory = nullptr;
	vkMapMemory(vulkanData->device, uniformBufferMemory, 0, sizeof(Shader::DrawImageShaderPipeline::UniformBuffer), 0, reinterpret_cast<void **>(&uniformMemory));
	memcpy(uniformMemory, &buffer, sizeof(buffer));
	vkUnmapMemory(vulkanData->device, uniformBufferMemory);
}

void Engine::DrawImage::draw(VkCommandBuffer commandBuffer, int32_t frameIdx) {

	updateUniforms();

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

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, drawImagePipeline);

	VkBuffer mBuffers[] = { vertexBuffer };
	VkDeviceSize mOffsets[] = { 0 };

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, mBuffers, mOffsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shaderPipeline->getPipelineLayout(), 0, 1, shaderPipeline->getDescriptorSets(), 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//VkImageBlit blitData = {};

	//blitData.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//blitData.srcSubresource.baseArrayLayer = 0;
	//blitData.srcSubresource.layerCount = 1;
	//blitData.srcSubresource.mipLevel = 0;

	//blitData.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//blitData.dstSubresource.baseArrayLayer = 0;
	//blitData.dstSubresource.layerCount = 1;
	//blitData.dstSubresource.mipLevel = 0;

	//blitData.srcOffsets[1] = { (int32_t)this->sourceExtent.width, (int32_t)this->sourceExtent.height, 1 };
	//blitData.dstOffsets[1] = { (int32_t)vulkanData->mSwapChainImageExtent.width, (int32_t)vulkanData->mSwapChainImageExtent.height, 1 };

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//transitionImageLayout(vulkanData->swapchainImage[frameIdx], vulkanData->mSwapChainImageFormat,
	//	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	//	VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_WRITE_BIT,
	//	commandBuffer);

	//vkCmdBlitImage(commandBuffer,
	//	sourceImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
	//	vulkanData->swapchainImage[frameIdx], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitData, VK_FILTER_LINEAR);
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


