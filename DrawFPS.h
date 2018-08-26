#include "DrawTextLine.h"

#pragma once
namespace Engine {
	class DrawFPS {
		const VulkanEngineApplication::VulkanData * vulkanData;
		const VulkanEngineApplication::VulkanEngineData *vulkanEngineData;

		DrawTextLine *drawText;
	public:
		DrawFPS(const VulkanEngineApplication::VulkanData * vulkanData, const VulkanEngineApplication::VulkanEngineData *vulkanEngineData);
		~DrawFPS();

		void resize(void);
		void initialize(const Asset::AssetLoader *assetLoader);
		void draw(VkCommandBuffer commandBuffer);
	};
}

