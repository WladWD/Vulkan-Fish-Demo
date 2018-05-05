#include "InitStruct.h"
#include "AssetLoader.h"

#pragma once
namespace Shader {
	class ShaderStageBase {
	protected:
		const VulkanEngineApplication::VulkanData *vulkanData;
		VkPipelineShaderStageCreateInfo shaderInfo;

	private:
		void createModule(int64_t size, uint8_t *data);
		void initialize(void);
	public:
		ShaderStageBase(
			const char *shaderResourceName, 
			const Asset::AssetLoader *asset,
			const VulkanEngineApplication::VulkanData *vulkanData, 
			VkShaderStageFlagBits stage,
			const char *entryName = "main");
		virtual ~ShaderStageBase();

		VkPipelineShaderStageCreateInfo getShaderStageInfo() const;
	};
}

