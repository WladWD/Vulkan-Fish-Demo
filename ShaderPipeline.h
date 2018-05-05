#include "ShaderStageBase.h"

#pragma once
namespace Shader {
	class ShaderPipeline {
		std::vector<ShaderStageBase *> stages;
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	protected:
		const VulkanEngineApplication::VulkanData *vulkanData;
		VkPipelineLayout pipelineLayout;

		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

		void addStage(ShaderStageBase *stage);
	public:
		ShaderPipeline(const VulkanEngineApplication::VulkanData *vulkanData);
		virtual ~ShaderPipeline();

		const VkPipelineShaderStageCreateInfo *getShaderStages(void) const;
		size_t getShaderStagesCount(void) const;

		const VkDescriptorSet *getDescriptorSets(void) const;
		size_t getDescriptorSetsCount(void) const;
		
		const VkDescriptorSetLayout *getDescriptorSetLayouts(void) const;
		size_t getDescriptorSetLayoutsCount(void) const;

		VkDescriptorPool getDescriptorPool(void) const;

		VkPipelineLayout getPipelineLayout(void) const;
	};
}

