#include "DrawImageShaderPipeline.h"
#include "FragmentShaderImageDraw.h"
#include "DrawImageUniformStruct.h"
#include "DrawImagePushConstant.h"


Shader::DrawImageShaderPipeline::DrawImageShaderPipeline(
	const Asset::AssetLoader * asset,
	const VulkanEngineApplication::VulkanData * vulkanData, const ConstBuffer &constBuffer): ShaderPipeline(vulkanData) {

	auto entry = ConstBuffer::getEntry();

	addStage(new ShaderStageBase("Resources\\Shaders\\ImageDraw\\vert.spv", asset, vulkanData, VK_SHADER_STAGE_VERTEX_BIT,
	{0}, entry.data(), &constBuffer, sizeof(constBuffer)));
	addStage(new FragmentShaderImageDraw("Resources\\Shaders\\ImageDraw\\frag.spv", asset, vulkanData, VK_SHADER_STAGE_FRAGMENT_BIT,
	{1}, entry.data(), &constBuffer, sizeof(constBuffer)));

	initializeDescriptorSetLayout();
	initializeDescriptorPool();
	initializeDescriptorSet();
	initializePipelineLayout();
	initializeUniformBuffer();
}

Shader::DrawImageShaderPipeline::~DrawImageShaderPipeline() {
	vkUnmapMemory(vulkanData->device, uniformBufferMemory);
	
	vkDestroyBuffer(vulkanData->device, uniformBuffer, nullptr);
	vkFreeMemory(vulkanData->device, uniformBufferMemory, nullptr);
}

VkBuffer Shader::DrawImageShaderPipeline::getUniformRotationBuffer(void) {
	return uniformBuffer;
}

void Shader::DrawImageShaderPipeline::setRotationUniformBlockValue(const UniformBuffer &buffer) {
	memcpy(rotationUniformBuffer, &buffer, sizeof(buffer));
}

void Shader::DrawImageShaderPipeline::initializeUniformBuffer(void) {
	VkDeviceSize uniformBufferSize = sizeof(Shader::DrawImageShaderPipeline::UniformBuffer);
	VulkanInitialize::createBuffer(vulkanData, uniformBufferSize,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		uniformBuffer, uniformBufferMemory);

	vkMapMemory(vulkanData->device, uniformBufferMemory, 0, sizeof(Shader::DrawImageShaderPipeline::UniformBuffer), 0, reinterpret_cast<void **>(&rotationUniformBuffer));
}

void Shader::DrawImageShaderPipeline::initializePipelineLayout(void) {

	std::array<VkPushConstantRange, 1> pushRange = {};
	pushRange[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	pushRange[0].offset = offsetof(PushConstantBuffer, multipleColor);
	pushRange[0].size = sizeof(PushConstantBuffer::multipleColor);


	VkPipelineLayoutCreateInfo mPipelineLayoutCreateInfo = {};
	mPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	mPipelineLayoutCreateInfo.setLayoutCount = static_cast<decltype(mPipelineLayoutCreateInfo.setLayoutCount)>(descriptorSetLayouts.size());
	mPipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
	mPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	mPipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
	mPipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushRange.size());
	mPipelineLayoutCreateInfo.pPushConstantRanges = pushRange.data();

	VkResult res = vkCreatePipelineLayout(vulkanData->device, &mPipelineLayoutCreateInfo, nullptr, &pipelineLayout);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to create pipeline layout!");
	}
}

void Shader::DrawImageShaderPipeline::initializeDescriptorPool(void) {
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].descriptorCount = 1;
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	poolSizes[1].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

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

void Shader::DrawImageShaderPipeline::initializeDescriptorSet(void) {
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

void Shader::DrawImageShaderPipeline::initializeDescriptorSetLayout(void) {
	VkDescriptorSetLayoutBinding samplerlayoutBinding = {};
	samplerlayoutBinding.binding = 1;
	samplerlayoutBinding.descriptorCount = 1;
	samplerlayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerlayoutBinding.pImmutableSamplers = nullptr;
	samplerlayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding layoutDescriptor = {};
	layoutDescriptor.binding = 0;
	layoutDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutDescriptor.descriptorCount = 1;
	layoutDescriptor.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	layoutDescriptor.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { layoutDescriptor, samplerlayoutBinding };

	VkDescriptorSetLayoutCreateInfo descriptorSetInfo = {};
	descriptorSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	descriptorSetInfo.pBindings = bindings.data();

	descriptorSetLayouts.resize(1);
	if (vkCreateDescriptorSetLayout(vulkanData->device, &descriptorSetInfo, nullptr, descriptorSetLayouts.data()) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed create descriptor set");
	}
}
