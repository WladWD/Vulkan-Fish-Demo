#include "DrawImage.h"
#include "DrawFPS.h"
#include "LoadModel.h"

#pragma once
namespace Engine 
{
	class VulkanEngine {
		const VulkanEngineApplication::VulkanData * vulkanData;
		VulkanEngineApplication::VulkanEngineData vulkanEngineData;
		
		std::shared_ptr<Draw::Model> scene;
		DrawImage *drawImage;
		DrawFPS *drawFPS;

		void initializeEngine(const Asset::AssetLoader *assetLoader);
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

//SPRIV-V documentetion
//https://vulkan.lunarg.com/doc/sdk/1.0.26.0/linux/spirv_toolchain.html
