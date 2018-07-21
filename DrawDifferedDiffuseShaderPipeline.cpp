#include "DrawDifferedDiffuseShaderPipeline.h"


Shader::DrawDifferedDiffuseShaderPipeline::DrawDifferedDiffuseShaderPipeline(
	const Asset::AssetLoader * asset, 
	const VulkanEngineApplication::VulkanData * vulkanData):
	DrawDifferedShaderPipeline(asset, vulkanData) {

	addStage(new ShaderStageBase("Resources\\Shaders\\DrawDiffered\\spriv-v\\vert.spv", asset, vulkanData, VK_SHADER_STAGE_VERTEX_BIT));
	addStage(new ShaderStageBase("Resources\\Shaders\\DrawDiffered\\spriv-v\\frag.spv", asset, vulkanData, VK_SHADER_STAGE_FRAGMENT_BIT));

	initializeDescriptorSetLayout(VK_SHADER_STAGE_VERTEX_BIT);
	initializeTextureDescriptorSetLayout();
	initializeDescriptorPool(1);
	initializeDescriptorSet();
	initializePipelineLayout();
	initializeUniformBuffer();
}

//Shader::DrawDifferedDiffuseShaderPipeline::~DrawDifferedDiffuseShaderPipeline() {
//}

void Shader::DrawDifferedDiffuseShaderPipeline::initializeTextureDescriptorSetLayout(void) {
	VkDescriptorSetLayoutBinding layoutDescriptor = {};
	layoutDescriptor.binding = 0;
	layoutDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layoutDescriptor.descriptorCount = 1;
	layoutDescriptor.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	layoutDescriptor.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 1> bindings = { layoutDescriptor };

	VkDescriptorSetLayoutCreateInfo descriptorSetInfo = {};
	descriptorSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	descriptorSetInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(vulkanData->device, &descriptorSetInfo, nullptr, &descriptorSetLayouts.data()[1]) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed create descriptor set");
	}
}

void Shader::DrawDifferedDiffuseShaderPipeline::setDiffuseTexture(const VkDescriptorImageInfo & imageInfo) {
	VkWriteDescriptorSet writeDescriptors = { };
	writeDescriptors.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptors.dstSet = getDescriptorSets()[1];
	writeDescriptors.dstBinding = 0;
	writeDescriptors.dstArrayElement = 0;
	writeDescriptors.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptors.descriptorCount = 1;
	writeDescriptors.pBufferInfo = nullptr;
	writeDescriptors.pImageInfo = &imageInfo;
	writeDescriptors.pTexelBufferView = nullptr;

	updateDescriptors.push_back(writeDescriptors);
}

void Shader::DrawDifferedDiffuseShaderPipeline::updateTextures(void) {
	vkUpdateDescriptorSets(vulkanData->device, static_cast<uint32_t>(updateDescriptors.size()), updateDescriptors.data(), 0, nullptr);
	updateDescriptors.clear();
}
