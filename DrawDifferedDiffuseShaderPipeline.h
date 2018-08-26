#include "DrawDifferedShaderPipeline.h"

#pragma once
namespace Shader {
	class DrawDifferedDiffuseShaderPipeline: 
		public DrawDifferedShaderPipeline {
	protected:
		std::vector<VkWriteDescriptorSet> updateDescriptors;

		virtual void initializeTextureDescriptorSetLayout(void) override;
	public:
		DrawDifferedDiffuseShaderPipeline(
			const Asset::AssetLoader *asset,
			const VulkanEngineApplication::VulkanData *vulkanData);
		virtual ~DrawDifferedDiffuseShaderPipeline() override = default;

		void setDiffuseTexture(const VkDescriptorImageInfo &imageInfo);
		void updateTextures(void);
	};
}

