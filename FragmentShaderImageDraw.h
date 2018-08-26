#include "ShaderStageBase.h"
#include "DrawImageShaderPipeline.h"
#include "DrawImageConstBuffer.h"

#pragma once
namespace Shader {
	class DrawImageShaderPipeline::FragmentShaderImageDraw : public ShaderStageBase {
	public:
		FragmentShaderImageDraw(
			const char *shaderResourceName,
			const Asset::AssetLoader *asset,
			const VulkanEngineApplication::VulkanData *vulkanData,
			VkShaderStageFlagBits stage,
			std::vector<int32_t> specIdx = {},
			const VkSpecializationMapEntry *specEntry = nullptr,
			const void *specData = nullptr,
			uint32_t dataSize = 0,
			const char *entryName = "main");
		~FragmentShaderImageDraw();
	}; 
}

