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

	mGraphicsPipelineCreateInfoStruct.stageCount = static_cast<uint32_t>(shader->getShaderStagesCount());
	mGraphicsPipelineCreateInfoStruct.pStages = shader->getShaderStages();
	mGraphicsPipelineCreateInfoStruct.pVertexInputState = &mVertexInputStageCreateInfo;
	mGraphicsPipelineCreateInfoStruct.pInputAssemblyState = &mInputAssemblyStateCreateInfo;
	mGraphicsPipelineCreateInfoStruct.layout = shader->getPipelineLayout();

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
