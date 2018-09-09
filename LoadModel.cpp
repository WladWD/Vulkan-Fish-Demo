#include "LoadModel.h"

LoadManager::LoadModel::LoadModel(
	const Asset::AssetLoader * assetLoader,
	const VulkanEngineApplication::VulkanData * vulkanData,
	const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
	const std::shared_ptr<Draw::Model> &scene) :
	scene(scene), vulkanData(vulkanData), vulkanEngineData(vulkanEngineData),
	vertexBuffer(VK_NULL_HANDLE), indexBuffer(VK_NULL_HANDLE),
	vertexBufferMemory(VK_NULL_HANDLE), indexBufferMemory(VK_NULL_HANDLE) {

	imageLoader = std::shared_ptr<ImageManager::ImageLoader>
		(new ImageManager::ImageLoader(assetLoader, vulkanData));
}

void LoadManager::LoadModel::packScene(void) {
	VkDeviceSize vertexBufferSize = packVertexBuffer();
	VkDeviceSize indexBufferSize = packIndexBuffer();

	scene->buffer = std::unique_ptr<Draw::ModelBuffer>(
		new Draw::ModelBuffer(
			vulkanData,
			vertexBuffer,
			indexBuffer,
			vertexBufferMemory,
			indexBufferMemory,
			vertexBufferSize,
			indexBufferSize
		));
}

const std::shared_ptr<Draw::Model>& LoadManager::LoadModel::getLoadedScene(void) {
	return scene;
}
