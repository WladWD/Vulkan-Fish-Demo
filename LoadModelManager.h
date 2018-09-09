#include "LoadModelAssimp.h"
#include "LoadModelNative.h"

#pragma once
namespace LoadManager {
	class LoadModelManager {
		//Use abstract base class #ifdef USE_ASSIMP
		std::unique_ptr<LoadModel> loadModel;
		std::shared_ptr<Draw::Model> scene;
		
	public:
		LoadModelManager(const Asset::AssetLoader * assetLoader, 
			const VulkanEngineApplication::VulkanData * vulkanData,
			const VulkanEngineApplication::VulkanEngineData *vulkanEngineData);
		~LoadModelManager() = default;

		void addModel(std::string model);
		void packScene(void);
		const std::shared_ptr<Draw::Model> &getLoadedScene(void);
	};
}
//TODO create base class for loading models

