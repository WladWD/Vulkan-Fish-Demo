#include "ShaderStageBase.h"


Shader::ShaderStageBase::ShaderStageBase(
	const char * shaderResourceName,
	const Asset::AssetLoader *asset,
	const VulkanEngineApplication::VulkanData *vulkanData, 
	VkShaderStageFlagBits stage,
	const char *entryName) : vulkanData(vulkanData) {
	assert(asset != nullptr);

	int64_t size; uint8_t *data;
	asset->loadAssetByName(shaderResourceName, &data, size);
	initialize();
	shaderInfo.stage = stage; 
	shaderInfo.pName = entryName;
	createModule(size, data);
	delete[] data;
}

Shader::ShaderStageBase::~ShaderStageBase() {
	vkDestroyShaderModule(vulkanData->device, shaderInfo.module, nullptr);
}

void Shader::ShaderStageBase::createModule(int64_t size, uint8_t * data) {
	VkShaderModuleCreateInfo shaderCreateInfo = {};
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.codeSize = static_cast<decltype(shaderCreateInfo.codeSize)>(size);
	shaderCreateInfo.flags = 0;
	shaderCreateInfo.pCode = reinterpret_cast<uint32_t *>(data);
	shaderCreateInfo.pNext = nullptr;

	VkResult result;
	assert((result = vkCreateShaderModule(vulkanData->device, &shaderCreateInfo, nullptr, &shaderInfo.module)) == VK_SUCCESS);
}

void Shader::ShaderStageBase::initialize(void) {
	shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderInfo.flags = 0;
	shaderInfo.pNext = nullptr;
	shaderInfo.pSpecializationInfo = nullptr;
}

VkPipelineShaderStageCreateInfo Shader::ShaderStageBase::getShaderStageInfo() const {
	return shaderInfo;
}
