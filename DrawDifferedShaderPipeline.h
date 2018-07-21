#include "ShaderPipeline.h"
#include "VertexStruct.h"
#include "VulkanCreater.h"

#pragma once
namespace Shader {
	class DrawDifferedShaderPipeline: public ShaderPipeline {
	public:
		struct UniformBuffer;
		struct PushConstantBuffer;

	protected:
		VkBuffer uniformBuffer;
		VkDeviceMemory uniformBufferMemory;

		UniformBuffer *uniformBufferData;
		std::shared_ptr<PushConstantBuffer> pushConstantBufferData;

		virtual void initializeTextureDescriptorSetLayout(void) = 0;
		void initializeUniformBuffer(void);
		void initializeDescriptorPool(uint32_t texCount);
		void initializePipelineLayout(void);
		void initializeDescriptorSet(void);
		void initializeDescriptorSetLayout(VkShaderStageFlags shaderStage);
	public:
		DrawDifferedShaderPipeline(
			const Asset::AssetLoader *asset,
			const VulkanEngineApplication::VulkanData *vulkanData);
		virtual ~DrawDifferedShaderPipeline() override;

		UniformBuffer *getUniformBuffer(void);
		std::shared_ptr<PushConstantBuffer> &getPushConstantBuffer(void);

		void updateConstantBufferData(VkCommandBuffer commandBuffer);
	};
}

