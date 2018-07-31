#include "DrawData.h"
#include "VulkanEngineData.h"
#include "DrawDifferedFramebuffer.h"

#pragma once
namespace DrawDiffered {
	class DrawDifferedBase {
	protected:
		const VulkanEngineApplication::VulkanData * vulkanData;
		const VulkanEngineApplication::VulkanEngineData *vulkanEngineData;
		const std::shared_ptr<Draw::Model> model;
		uint32_t drawType;
		VkPipeline drawDifferredPipeline;

		virtual void initializePipeline(const std::shared_ptr<Draw::DrawDifferedFramebuffer> &framebuffer) = 0;
	public:
		DrawDifferedBase(
			const VulkanEngineApplication::VulkanData * vulkanData,
			const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
			const std::shared_ptr<Draw::Model> &model,
			Draw::DrawDataTypes drawType);
		virtual ~DrawDifferedBase() = default;

		virtual void draw(VkCommandBuffer commandBuffer) = 0;
	};
}

