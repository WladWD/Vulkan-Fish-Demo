#include "DrawDifferedBase.h"
#include "DrawDifferedDiffuseShaderPipeline.h"

#pragma once
namespace DrawDiffered {
	class DrawDifferedDiffuse: public DrawDifferedBase {
		std::unique_ptr<Shader::DrawDifferedDiffuseShaderPipeline> shader;

		void updateUniforms(void);
		void setMaterial(VkCommandBuffer commandBuffer, uint32_t materialIdx);
		virtual void initializePipeline(void) override;
	public:
		DrawDifferedDiffuse(const VulkanEngineApplication::VulkanData * vulkanData,
			const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
			const std::shared_ptr<Draw::Model> &model,
			const Asset::AssetLoader *assetLoader,
			const std::shared_ptr<Draw::DrawDifferedFramebuffer> &framebuffer);
		~DrawDifferedDiffuse() override = default;
	
		virtual void draw(VkCommandBuffer commandBuffer) override;
	};
}

