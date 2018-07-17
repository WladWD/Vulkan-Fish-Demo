#include "LoadModel.h"

LoadManager::LoadModel::LoadModel(
	const Asset::AssetLoader * assetLoader, 
	const VulkanEngineApplication::VulkanData * vulkanData,
	const VulkanEngineApplication::VulkanEngineData *vulkanEngineData) {

	scene = std::shared_ptr<Draw::Model>(new Draw::Model());
	loadModel = std::unique_ptr<LoadModelAssimp>(new LoadModelAssimp(assetLoader, vulkanData, vulkanEngineData, scene));
}

void LoadManager::LoadModel::addModel(std::string model) {
	loadModel->addModel(model);
}

void LoadManager::LoadModel::packScene(void) {
	loadModel->packScene();
}

const std::shared_ptr<Draw::Model>& LoadManager::LoadModel::getLoadedScene(void) {
	return loadModel->getLoadedScene();
}
