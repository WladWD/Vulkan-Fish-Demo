#include "DrawDifferedDiffuse.h"
#include "DrawDifferedFramebuffer.h"
#include <map>

#pragma once
namespace DrawDiffered {
	class DrawDifferedManager {
		const VulkanEngineApplication::VulkanData * vulkanData;
		std::unordered_map<Draw::DrawDataTypes, std::unique_ptr<DrawDifferedBase>> drawDiffered;
		std::shared_ptr<Draw::DrawDifferedFramebuffer> framebuffer;
		std::shared_ptr<Draw::Model> model;

	public:
		DrawDifferedManager(const VulkanEngineApplication::VulkanData * vulkanData,
			const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
			const std::shared_ptr<Draw::Model> &model,
			const Asset::AssetLoader *assetLoader);
		~DrawDifferedManager() = default;

		void draw(VkCommandBuffer commandBuffer);
	};
}


