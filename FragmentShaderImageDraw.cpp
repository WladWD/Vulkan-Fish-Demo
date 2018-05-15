#include "FragmentShaderImageDraw.h"


Shader::DrawImageShaderPipeline::FragmentShaderImageDraw::FragmentShaderImageDraw(
	const char *shaderResourceName,
	const Asset::AssetLoader *asset,
	const VulkanEngineApplication::VulkanData *vulkanData,
	VkShaderStageFlagBits stage,
	std::vector<int32_t> specIdx,
	const VkSpecializationMapEntry *specEntry,
	const void *specData,
	uint32_t dataSize,
	const char *entryName): ShaderStageBase(shaderResourceName, asset, vulkanData, stage, specIdx, specEntry, specData, dataSize, entryName) {
}

Shader::DrawImageShaderPipeline::FragmentShaderImageDraw::~FragmentShaderImageDraw()
{
}
