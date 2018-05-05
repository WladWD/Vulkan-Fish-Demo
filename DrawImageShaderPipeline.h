#include "ShaderPipeline.h"
#include "DrawImageVertex.h"

#pragma once
namespace Shader {
	class DrawImageShaderPipeline: public ShaderPipeline {
		class FragmentShaderImageDraw;

		void initializePipelineLayout(void);
		void initializeDescriptorPool(void);
		void initializeDescriptorSet(void);
		void initializeDescriptorSetLayout(void);
	public:
		DrawImageShaderPipeline(
			const Asset::AssetLoader *asset,
			const VulkanEngineApplication::VulkanData *vulkanData);
		~DrawImageShaderPipeline();

		struct UniformBuffer;
	};


	class DrawImageShaderPipeline::FragmentShaderImageDraw: public ShaderStageBase {
	public:
		FragmentShaderImageDraw(
			const char *shaderResourceName,
			const Asset::AssetLoader *asset,
			const VulkanEngineApplication::VulkanData *vulkanData,
			VkShaderStageFlagBits stage,
			glm::vec4 clearColor,
			const char *entryName = "main");
		~FragmentShaderImageDraw();
	};
}

