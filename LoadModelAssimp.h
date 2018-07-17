#include "LoadDiffuseMesh.h"
#include "VulkanEngineData.h"

#pragma once
namespace LoadManager {
	class LoadModelAssimp {
		std::unique_ptr<LoadManager::MaterialLoader> materialLoader;
		std::shared_ptr<ImageManager::ImageLoader> imageLoader;
		std::vector<std::unique_ptr<LoadMesh>> loadMesh;

		std::shared_ptr<Draw::Model> scene;

	public:
		LoadModelAssimp(const Asset::AssetLoader * assetLoader,
			const VulkanEngineApplication::VulkanData * vulkanData,
			const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
			const std::shared_ptr<Draw::Model> &scene);
		~LoadModelAssimp() = default;

		void addModel(std::string model);
		void packScene(void);
		const std::shared_ptr<Draw::Model> &getLoadedScene(void);
	};
}

