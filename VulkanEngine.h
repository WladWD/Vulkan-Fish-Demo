#include "DrawImage.h"
#include "DrawFPS.h"

#pragma once
namespace Engine 
{
	class VulkanEngine {
		const VulkanEngineApplication::VulkanData * vulkanData;
		VulkanEngineApplication::VulkanEngineData vulkanEngineData;
		
		DrawImage *drawImage;
		DrawFPS *drawFPS;

		void initializeEngine(void);
		void drawToSwapChainTexture(void);
	public:
		VulkanEngine(const VulkanEngineApplication::VulkanData * vulkanData);
		~VulkanEngine();
		
		void initialize(const Asset::AssetLoader *assetLoader);
		void resize(void);
		
		void pause(void);
		void resume(void);
		
		void draw(void);
	};
}

