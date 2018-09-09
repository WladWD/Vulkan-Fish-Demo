#include "LoadFormattedModel.h"
#include "LoadMeshAssimp.h"

#ifdef USE_ASSIMP
#pragma once
namespace LoadManager {
	class LoadModelAssimp: public LoadFormattedModel {
		std::unique_ptr<LoadManager::MaterialLoader> materialLoader;

	public:
		LoadModelAssimp(const Asset::AssetLoader * assetLoader,
			const VulkanEngineApplication::VulkanData * vulkanData,
			const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
			const std::shared_ptr<Draw::Model> &scene);
		virtual ~LoadModelAssimp() = default;

		virtual void addModel(const std::string &model) override;
	};
}
#endif


