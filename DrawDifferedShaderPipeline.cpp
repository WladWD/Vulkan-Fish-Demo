#include "DrawDifferedShaderPipeline.h"
#include "DrawDifferedBuffer.h"

Shader::DrawDifferedShaderPipeline::DrawDifferedShaderPipeline(
	const Asset::AssetLoader * asset, 
	const VulkanEngineApplication::VulkanData * vulkanData):
	ShaderPipeline(vulkanData), uniformBufferData(nullptr), pushConstantBufferData(nullptr) {

	pushConstantBufferData = std::shared_ptr<PushConstantBuffer>(new PushConstantBuffer());

	descriptorSets.resize(2);
	descriptorSetLayouts.resize(2);
}

Shader::DrawDifferedShaderPipeline::~DrawDifferedShaderPipeline() {
	vkUnmapMemory(vulkanData->device, uniformBufferMemory);

	vkDestroyBuffer(vulkanData->device, uniformBuffer, nullptr);
	vkFreeMemory(vulkanData->device, uniformBufferMemory, nullptr);
}

void Shader::DrawDifferedShaderPipeline::initializeUniformBuffer(void) {
	VkDeviceSize uniformBufferSize = sizeof(Shader::DrawDifferedShaderPipeline::UniformBuffer);
	VulkanInitialize::createBuffer(vulkanData, uniformBufferSize,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		uniformBuffer, uniformBufferMemory);

	vkMapMemory(vulkanData->device, uniformBufferMemory, 0, uniformBufferSize, 0, reinterpret_cast<void **>(&uniformBufferData));
}

void Shader::DrawDifferedShaderPipeline::initializePipelineLayout(void) {
	std::array<VkPushConstantRange, 1> pushRange = {};
	pushRange[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	pushRange[0].offset = 0;
	pushRange[0].size = sizeof(Shader::DrawDifferedShaderPipeline::PushConstantBuffer);

	VkPipelineLayoutCreateInfo mPipelineLayoutCreateInfo = {};
	mPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	mPipelineLayoutCreateInfo.setLayoutCount = static_cast<decltype(mPipelineLayoutCreateInfo.setLayoutCount)>(descriptorSetLayouts.size());
	mPipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
	mPipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushRange.size());
	mPipelineLayoutCreateInfo.pPushConstantRanges = pushRange.data();

	VkResult res = vkCreatePipelineLayout(vulkanData->device, &mPipelineLayoutCreateInfo, nullptr, &pipelineLayout);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to create pipeline layout!");
	}
}

void Shader::DrawDifferedShaderPipeline::initializeDescriptorPool(uint32_t texCount) {
	std::vector<VkDescriptorPoolSize> poolSizes(1 + !!texCount);
	poolSizes[0].descriptorCount = 1;
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	if (texCount) {
		poolSizes[1].descriptorCount = texCount;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	}

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.pNext = nullptr;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(descriptorSets.size());

	if (vkCreateDescriptorPool(vulkanData->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed creating descriptor pool");
	}
}

void Shader::DrawDifferedShaderPipeline::initializeDescriptorSet(void) {
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pSetLayouts = descriptorSetLayouts.data();
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<decltype(allocInfo.descriptorSetCount)>(descriptorSetLayouts.size());

	if (vkAllocateDescriptorSets(vulkanData->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("Error allocate descriptor set");
	}
}

void Shader::DrawDifferedShaderPipeline::initializeDescriptorSetLayout(VkShaderStageFlags shaderStage) {
	VkDescriptorSetLayoutBinding layoutDescriptor = {};
	layoutDescriptor.binding = 0;
	layoutDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutDescriptor.descriptorCount = 1;
	layoutDescriptor.stageFlags = shaderStage;// VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_GEOMETRY_BIT;
	layoutDescriptor.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 1> bindings = { layoutDescriptor };

	VkDescriptorSetLayoutCreateInfo descriptorSetInfo = {};
	descriptorSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	descriptorSetInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(vulkanData->device, &descriptorSetInfo, nullptr, &descriptorSetLayouts.data()[0]) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed create descriptor set");
	}
}

Shader::DrawDifferedShaderPipeline::UniformBuffer *Shader::DrawDifferedShaderPipeline::getUniformBuffer(void) {
	return uniformBufferData;
}

std::shared_ptr<Shader::DrawDifferedShaderPipeline::PushConstantBuffer> &Shader::DrawDifferedShaderPipeline::getPushConstantBuffer(void) {
	return pushConstantBufferData;
}

void Shader::DrawDifferedShaderPipeline::updateConstantBufferData(VkCommandBuffer commandBuffer) {
	vkCmdPushConstants(commandBuffer, getPipelineLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, 
		sizeof(Shader::DrawDifferedShaderPipeline::PushConstantBuffer), pushConstantBufferData.get());
}

