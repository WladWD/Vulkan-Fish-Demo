#include "DrawDifferedDiffuse.h"
#include "DrawDifferedBuffer.h"
#include "VertexStruct.h"
#include <chrono>

DrawDiffered::DrawDifferedDiffuse::DrawDifferedDiffuse(
	const VulkanEngineApplication::VulkanData * vulkanData,
	const VulkanEngineApplication::VulkanEngineData * vulkanEngineData,
	const std::shared_ptr<Draw::Model>& model,
	const Asset::AssetLoader *assetLoader,
	const std::shared_ptr<Draw::DrawDifferedFramebuffer> &framebuffer):
	DrawDifferedBase(vulkanData, vulkanEngineData, framebuffer, model, Draw::DrawDataTypes::DrawDataDiffuse), shader(nullptr) {

	shader = std::make_unique<Shader::DrawDifferedDiffuseShaderPipeline>(assetLoader, vulkanData);
	initializePipeline();
}

void DrawDiffered::DrawDifferedDiffuse::initializePipeline(void) {
	auto mVertexAttributeDescription = Draw::Vertex::getAttributeDescription();
	auto mVertexBindingDescription = Draw::Vertex::getBindingDescription();

	VkPipelineVertexInputStateCreateInfo mVertexInputStageCreateInfo = {};
	mVertexInputStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	mVertexInputStageCreateInfo.pVertexAttributeDescriptions = mVertexAttributeDescription.data();
	mVertexInputStageCreateInfo.pVertexBindingDescriptions = &mVertexBindingDescription;
	mVertexInputStageCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(mVertexAttributeDescription.size());
	mVertexInputStageCreateInfo.vertexBindingDescriptionCount = 1U;

	VkPipelineInputAssemblyStateCreateInfo mInputAssemblyStateCreateInfo = {};
	mInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	mInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
	mInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	//VkViewport mViewport = {};
	//mViewport.x = 0.0f;
	//mViewport.y = 0.0f;
	//mViewport.width = static_cast<float>(framebuffer->getFramebufferSize().width);
	//mViewport.height = static_cast<float>(framebuffer->getFramebufferSize().height);
	//mViewport.minDepth = 0.0f;
	//mViewport.maxDepth = 1.0f;

	//VkRect2D mScissorRect = {};
	//mScissorRect.extent.width = static_cast<decltype(mScissorRect.extent.width)>(mViewport.width);
	//mScissorRect.extent.height = static_cast<decltype(mScissorRect.extent.height)>(mViewport.height);
	//mScissorRect.offset = { 0, 0 };

	//VkPipelineViewportStateCreateInfo mPipelineViewportStateCreateInfo = {};
	//mPipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	//mPipelineViewportStateCreateInfo.pScissors = &mScissorRect;
	//mPipelineViewportStateCreateInfo.scissorCount = 1;
	//mPipelineViewportStateCreateInfo.pViewports = &mViewport;
	//mPipelineViewportStateCreateInfo.viewportCount = 1;

	//VkPipelineRasterizationStateCreateInfo mRasterizationStateCreateInfo = {};
	//mRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	//mRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	//mRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	//mRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;//VK_POLYGON_MODE_LINE VK_POLYGON_MODE_FILL
	//mRasterizationStateCreateInfo.lineWidth = 1.0f;
	//mRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;//VK_CULL_MODE_BACK_BIT
	//mRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	//mRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	//mRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	//mRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	//mRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

	//VkPipelineMultisampleStateCreateInfo mMultisampleStateCreateInfo = {};
	//mMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	//mMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	//mMultisampleStateCreateInfo.minSampleShading = 1.0f;
	//mMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	//mMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
	//mMultisampleStateCreateInfo.pSampleMask = nullptr;
	//mMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;

	//VkPipelineColorBlendAttachmentState mColorBlendAttachmentState = {};
	//mColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
	//	VK_COLOR_COMPONENT_G_BIT |
	//	VK_COLOR_COMPONENT_B_BIT |
	//	VK_COLOR_COMPONENT_A_BIT;
	//mColorBlendAttachmentState.blendEnable = VK_FALSE;
	//mColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	//mColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	//mColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	//mColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//mColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	//mColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	//std::array<VkPipelineColorBlendAttachmentState, 3> pipelineColorAttachmentState = { mColorBlendAttachmentState, mColorBlendAttachmentState, mColorBlendAttachmentState };

	//VkPipelineColorBlendStateCreateInfo mColorBlendStateCreateInfo = {};
	//mColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	//mColorBlendStateCreateInfo.attachmentCount = static_cast<decltype(mColorBlendStateCreateInfo.attachmentCount)>(pipelineColorAttachmentState.size());
	//mColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	//mColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	//mColorBlendStateCreateInfo.pAttachments = pipelineColorAttachmentState.data();
	//mColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	//mColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	//mColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	//mColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	//VkPipelineDepthStencilStateCreateInfo depthStancilCreateInfo = {};
	//depthStancilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	//depthStancilCreateInfo.depthTestEnable = VK_TRUE;
	//depthStancilCreateInfo.depthWriteEnable = VK_TRUE;
	//depthStancilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	//depthStancilCreateInfo.depthBoundsTestEnable = VK_FALSE;
	//depthStancilCreateInfo.minDepthBounds = 0.0f;
	//depthStancilCreateInfo.maxDepthBounds = 1.0f;
	//depthStancilCreateInfo.stencilTestEnable = VK_FALSE;
	//depthStancilCreateInfo.front = {};
	//depthStancilCreateInfo.back = {};

	mGraphicsPipelineCreateInfoStruct.stageCount = static_cast<uint32_t>(shader->getShaderStagesCount());
	mGraphicsPipelineCreateInfoStruct.pStages = shader->getShaderStages();
	mGraphicsPipelineCreateInfoStruct.pVertexInputState = &mVertexInputStageCreateInfo;
	mGraphicsPipelineCreateInfoStruct.pInputAssemblyState = &mInputAssemblyStateCreateInfo;
	mGraphicsPipelineCreateInfoStruct.layout = shader->getPipelineLayout();

	/*VkResult result = vkCreateGraphicsPipelines(vulkanData->device, VK_NULL_HANDLE, 1, &mGraphicsPipelineCreateInfoStruct, nullptr, &drawDifferredPipeline);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to create graphics pipeline!");
	}*/
	createGraphicsPipeline();
}

void DrawDiffered::DrawDifferedDiffuse::updateUniforms(void) {

	static auto begTime = std::chrono::high_resolution_clock::now();
	auto curTime = std::chrono::high_resolution_clock::now();
	auto deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(curTime - begTime).count();


	Shader::DrawDifferedDiffuseShaderPipeline::UniformBuffer *uniformBuffer = shader->getUniformBuffer();
	uniformBuffer->mWorld = model->mWorld;
	uniformBuffer->mWorldInvTransp = glm::transpose(glm::inverse(uniformBuffer->mWorld));
	uniformBuffer->mTexTransform = glm::mat4(1.0);

	float r = 5000.0f;
	uniformBuffer->cameraPosition = glm::vec4(r * glm::cos(deltaTime) * glm::cos(deltaTime), r * glm::sin(deltaTime), -5000.0 * glm::cos(deltaTime) * glm::sin(deltaTime), 0.0);//glm::vec4(0.0, 20.0, -40.0, 0.0);//
	uniformBuffer->mProjView = glm::perspective(1.0f, static_cast<float>(vulkanData->mSwapChainImageExtent.width) / static_cast<float>(vulkanData->mSwapChainImageExtent.height), 1.0f, 20000.0f);
	uniformBuffer->mProjView[1][1] *= -1;
	uniformBuffer->mProjView *= glm::lookAt(glm::vec3(uniformBuffer->cameraPosition), glm::vec3(0.0, 50.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
}

void DrawDiffered::DrawDifferedDiffuse::setMaterial(VkCommandBuffer commandBuffer, uint32_t materialIdx) {
	Draw::Material material = model->materials[materialIdx];
	const ImageManager::ImageData &imageData = vulkanEngineData->imageContainer->getImageDataByKey(material.diffuseTextureID);

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = imageData.imageView;
	imageInfo.sampler = vulkanEngineData->samplers->getMinMaxMag_Linear_UVW_Wrap(0, imageData.levelCount - 1);
	shader->setDiffuseTexture(imageInfo);
	shader->updateTextures();

	std::shared_ptr<Shader::DrawDifferedShaderPipeline::PushConstantBuffer> materialColor = shader->getPushConstantBuffer();
	materialColor->diffuseColor = material.diffuseColor;
	materialColor->emissiveColor = material.emissiveColor;
	materialColor->specularColor = material.specularColor;
	shader->updateConstantBufferData(commandBuffer);
}

void DrawDiffered::DrawDifferedDiffuse::draw(VkCommandBuffer commandBuffer) {
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, drawDifferredPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		shader->getPipelineLayout(), 0, 
		static_cast<uint32_t>(shader->getDescriptorSetsCount()),
		shader->getDescriptorSets(), 0, nullptr);

	updateUniforms();

	const Draw::DrawData &drawData = model->drawData[drawType];
	for (uint32_t i = 0; i < drawData.firstIndex.size(); ++i) {
		setMaterial(commandBuffer, drawData.materialIdx[i]);
		vkCmdDrawIndexed(commandBuffer, drawData.indexCount[i], 1, drawData.firstIndex[i], drawData.vertexOffset[i], 0);
	}
}
