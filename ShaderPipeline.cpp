#include "ShaderPipeline.h"


Shader::ShaderPipeline::ShaderPipeline(
	const VulkanEngineApplication::VulkanData *vulkanData): 
	vulkanData(vulkanData), 
	pipelineLayout(VK_NULL_HANDLE), 
	descriptorPool(VK_NULL_HANDLE) {
}

Shader::ShaderPipeline::~ShaderPipeline() {
	for (auto stage : stages) {
		delete stage;
	}

	for (auto layout : descriptorSetLayouts) {
		vkDestroyDescriptorSetLayout(vulkanData->device, layout, nullptr);
	}

	vkDestroyDescriptorPool(vulkanData->device, descriptorPool, nullptr);

	stages.clear();
	shaderStages.clear();
	descriptorSetLayouts.clear();

	vkDestroyPipelineLayout(vulkanData->device, pipelineLayout, nullptr);
}

void Shader::ShaderPipeline::addStage(ShaderStageBase * stage) {
	stages.push_back(stage);
	shaderStages.push_back(stage->getShaderStageInfo());
}

const VkPipelineShaderStageCreateInfo *Shader::ShaderPipeline::getShaderStages(void) const {
	return shaderStages.data();
}

size_t Shader::ShaderPipeline::getShaderStagesCount(void) const {
	return shaderStages.size();
}

const VkDescriptorSet * Shader::ShaderPipeline::getDescriptorSets(void) const {
	return descriptorSets.data();
}

size_t Shader::ShaderPipeline::getDescriptorSetsCount(void) const {
	return descriptorSets.size();
}

const VkDescriptorSetLayout * Shader::ShaderPipeline::getDescriptorSetLayouts(void) const {
	return descriptorSetLayouts.data();
}

size_t Shader::ShaderPipeline::getDescriptorSetLayoutsCount(void) const {
	return descriptorSetLayouts.size();
}

VkDescriptorPool Shader::ShaderPipeline::getDescriptorPool(void) const {
	return descriptorPool;
}

VkPipelineLayout Shader::ShaderPipeline::getPipelineLayout(void) const {
	return pipelineLayout;
}
