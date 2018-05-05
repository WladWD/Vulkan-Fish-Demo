#include "FragmentShaderImageDraw.h"


Shader::DrawImageShaderPipeline::FragmentShaderImageDraw::FragmentShaderImageDraw(
	const char * shaderResourceName,
	const Asset::AssetLoader * asset, 
	const VulkanEngineApplication::VulkanData * vulkanData,
	VkShaderStageFlagBits stage,
	glm::vec4 clearColor,
	const char * entryName): ShaderStageBase(shaderResourceName, asset, vulkanData, stage, entryName) {

	//TODO this
	shaderInfo.pSpecializationInfo;
}

Shader::DrawImageShaderPipeline::FragmentShaderImageDraw::~FragmentShaderImageDraw()
{
}
