#include "LoadModelAssimp.h"

#pragma once
namespace LoadManager {
	class LoadModel {
		//Use abstract base class #ifdef USE_ASSIMP
		std::unique_ptr<LoadModelAssimp> loadModel;
		std::shared_ptr<Draw::Model> scene;
		
	public:
		LoadModel(const Asset::AssetLoader * assetLoader, 
			const VulkanEngineApplication::VulkanData * vulkanData,
			const VulkanEngineApplication::VulkanEngineData *vulkanEngineData);
		~LoadModel() = default;

		void addModel(std::string model);
		void packScene(void);
		const std::shared_ptr<Draw::Model> &getLoadedScene(void);
	};
}
//TODO create base class for loading models

