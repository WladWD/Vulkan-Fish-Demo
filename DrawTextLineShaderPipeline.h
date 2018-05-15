#include "ShaderPipeline.h"
#include "VulkanCreater.h"

#pragma once
namespace Shader {
	class DrawTextLineShaderPipeline : public ShaderPipeline {
	public:
		struct PushConstantBuffer;
	private:
		void initializePipelineLayout(void);
		void initializeDescriptorPool(void);
		void initializeDescriptorSet(void);
		void initializeDescriptorSetLayout(void);
	public:
		DrawTextLineShaderPipeline(
			const Asset::AssetLoader *asset,
			const VulkanEngineApplication::VulkanData *vulkanData);
		~DrawTextLineShaderPipeline() = default;
	};
}

