#include "InitStruct.h"
#include "AssetLoader.h"

#pragma once
namespace Shader {
	class ShaderStageBase {
	protected:
		const VulkanEngineApplication::VulkanData *vulkanData;
		VkPipelineShaderStageCreateInfo shaderInfo;
		VkSpecializationInfo specInfo;
	private:
		void createModule(int64_t size, uint8_t *data);
		void initializeSpecConstant(int32_t *specIdx,
			size_t specIdxCount,
			const VkSpecializationMapEntry *specEntry);
		void initialize(void);
	public:
		ShaderStageBase(
			const char *shaderResourceName, 
			const Asset::AssetLoader *asset,
			const VulkanEngineApplication::VulkanData *vulkanData, 
			VkShaderStageFlagBits stage,
			std::vector<int32_t> specIdx = {},
			const VkSpecializationMapEntry *specEntry = nullptr,
			const void *specData = nullptr,
			uint32_t dataSize = 0,
			const char *entryName = "main");
		virtual ~ShaderStageBase();

		VkPipelineShaderStageCreateInfo getShaderStageInfo() const;
	};
}

