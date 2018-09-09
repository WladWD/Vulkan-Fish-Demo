#include "LoadFormattedModel.h"

LoadManager::LoadFormattedModel::LoadFormattedModel(
	const Asset::AssetLoader * assetLoader,
	const VulkanEngineApplication::VulkanData * vulkanData,
	const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
	const std::shared_ptr<Draw::Model> &scene): LoadModel(assetLoader, vulkanData, vulkanEngineData, scene) { }

VkDeviceSize LoadManager::LoadFormattedModel::packVertexBuffer(void) {
	VkDeviceSize bufferSize = 0;
	uint32_t bufferOffset = 0;

	if (vertexBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(vulkanData->device, vertexBuffer, nullptr);
		vertexBuffer = VK_NULL_HANDLE;
	}

	if (vertexBufferMemory != VK_NULL_HANDLE) {
		vkFreeMemory(vulkanData->device, vertexBufferMemory, nullptr);
		vertexBufferMemory = VK_NULL_HANDLE;
	}

	for (const auto &loader: loadMesh) {
		bufferSize += loader.second->getRawVertexDataSize();
	}

	std::vector<int8_t> vertex(bufferSize);
	for (const auto &loader: loadMesh) {
		memcpy(vertex.data() + bufferOffset, loader.second->getRawVertexData(), loader.second->getRawVertexDataSize());
		bufferOffset += loader.second->getRawVertexDataSize();
	}

	VulkanInitialize::createInitializeDeviceLocalBuffer(
		vulkanData,
		vulkanData->commandPool,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferSize,
		vertex.data(),
		vertexBuffer,
		vertexBufferMemory
	);

	return bufferSize;
}

VkDeviceSize LoadManager::LoadFormattedModel::packIndexBuffer(void) {
	VkDeviceSize bufferSize = 0;
	uint32_t bufferOffset = 0;

	if (indexBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(vulkanData->device, indexBuffer, nullptr);
		indexBuffer = VK_NULL_HANDLE;
	}

	if (indexBufferMemory != VK_NULL_HANDLE) {
		vkFreeMemory(vulkanData->device, indexBufferMemory, nullptr);
		indexBufferMemory = VK_NULL_HANDLE;
	}

	for (const auto &loader : loadMesh) {
		bufferSize += loader.second->getRawIndexDataSize();
	}

	std::vector<int8_t> index(bufferSize);
	for (const auto &loader : loadMesh) {
		memcpy(index.data() + bufferOffset, loader.second->getRawIndexData(), loader.second->getRawIndexDataSize());
		bufferOffset += loader.second->getRawIndexDataSize();
	}

	VulkanInitialize::createInitializeDeviceLocalBuffer(
		vulkanData,
		vulkanData->commandPool,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferSize,
		index.data(),
		indexBuffer,
		indexBufferMemory
	);

	return bufferSize;
}
