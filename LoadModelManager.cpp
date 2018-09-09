#include "LoadModelManager.h"

LoadManager::LoadModelManager::LoadModelManager(
	const Asset::AssetLoader * assetLoader, 
	const VulkanEngineApplication::VulkanData * vulkanData,
	const VulkanEngineApplication::VulkanEngineData *vulkanEngineData) {

	scene = std::shared_ptr<Draw::Model>(new Draw::Model());
	//loadModel = std::unique_ptr<LoadModelAssimp>(new LoadModelAssimp(assetLoader, vulkanData, vulkanEngineData, scene));
	loadModel = std::unique_ptr<LoadModelNative>(new LoadModelNative(assetLoader, vulkanData, vulkanEngineData, scene));
}

void LoadManager::LoadModelManager::addModel(std::string model) {
	loadModel->addModel(model);
}

void LoadManager::LoadModelManager::packScene(void) {
	loadModel->packScene();
}

const std::shared_ptr<Draw::Model>& LoadManager::LoadModelManager::getLoadedScene(void) {
	return loadModel->getLoadedScene();
}
