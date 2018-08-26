#include "ShaderPipeline.h"
#include "DrawImageVertex.h"
#include "VulkanCreater.h"

#pragma once
namespace Shader {
	class DrawImageShaderPipeline: public ShaderPipeline {
	private:
		class FragmentShaderImageDraw;
	public:
		struct UniformBuffer;
		struct ConstBuffer;
		struct PushConstantBuffer;

	private:
		VkBuffer uniformBuffer;
		VkDeviceMemory uniformBufferMemory;
		void *rotationUniformBuffer;

		void initializeUniformBuffer(void);
		void initializePipelineLayout(void);
		void initializeDescriptorPool(void);
		void initializeDescriptorSet(void);
		void initializeDescriptorSetLayout(void);
	public:
		DrawImageShaderPipeline(
			const Asset::AssetLoader *asset,
			const VulkanEngineApplication::VulkanData *vulkanData,
			const ConstBuffer &constBuffer);
		~DrawImageShaderPipeline();

		VkBuffer getUniformRotationBuffer(void);
		void setRotationUniformBlockValue(const UniformBuffer &buffer);
	};
}

