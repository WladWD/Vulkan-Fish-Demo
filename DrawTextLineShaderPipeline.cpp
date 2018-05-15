#include "DrawTextLineShaderPipeline.h"
#include "DrawTextPushConstant.h"

Shader::DrawTextLineShaderPipeline::DrawTextLineShaderPipeline(
	const Asset::AssetLoader * asset,
	const VulkanEngineApplication::VulkanData * vulkanData): ShaderPipeline(vulkanData) {

	addStage(new ShaderStageBase("Resources\\Shaders\\DrawText\\vert.spv", asset, vulkanData, VK_SHADER_STAGE_VERTEX_BIT));
	addStage(new ShaderStageBase("Resources\\Shaders\\DrawText\\frag.spv", asset, vulkanData, VK_SHADER_STAGE_FRAGMENT_BIT));

	initializeDescriptorSetLayout();
	initializeDescriptorPool();
	initializeDescriptorSet();
	initializePipelineLayout();
}

void Shader::DrawTextLineShaderPipeline::initializePipelineLayout(void) {
	std::array<VkPushConstantRange, 1> pushRange = {};
	pushRange[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushRange[0].offset = offsetof(PushConstantBuffer, scaleOffset);
	pushRange[0].size = sizeof(PushConstantBuffer);

	//pushRange[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	//pushRange[1].offset = offsetof(PushConstantBuffer, data);
	//pushRange[1].size = sizeof(PushConstantBuffer::data);

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

void Shader::DrawTextLineShaderPipeline::initializeDescriptorPool(void) {
	std::array<VkDescriptorPoolSize, 1> poolSizes = {};
	poolSizes[0].descriptorCount = 1;
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.pNext = nullptr;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1;

	if (vkCreateDescriptorPool(vulkanData->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed creating descriptor pool");
	}
}

void Shader::DrawTextLineShaderPipeline::initializeDescriptorSet(void) {
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pSetLayouts = descriptorSetLayouts.data();
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<decltype(allocInfo.descriptorSetCount)>(descriptorSetLayouts.size());

	descriptorSets.resize(1);
	if (vkAllocateDescriptorSets(vulkanData->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("Error allocate descriptor set");
	}
}

void Shader::DrawTextLineShaderPipeline::initializeDescriptorSetLayout(void) {
	VkDescriptorSetLayoutBinding samplerlayoutBinding = {};
	samplerlayoutBinding.binding = 0;
	samplerlayoutBinding.descriptorCount = 1;
	samplerlayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerlayoutBinding.pImmutableSamplers = nullptr;
	samplerlayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 1> bindings = { samplerlayoutBinding };

	VkDescriptorSetLayoutCreateInfo descriptorSetInfo = {};
	descriptorSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	descriptorSetInfo.pBindings = bindings.data();

	descriptorSetLayouts.resize(1);
	if (vkCreateDescriptorSetLayout(vulkanData->device, &descriptorSetInfo, nullptr, descriptorSetLayouts.data()) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed create descriptor set");
	}
}
