#include "StoreSceneNative.h"

#ifdef USE_ASSIMP
Store::StoreSceneNative::StoreSceneNative(const VulkanEngineApplication::VulkanEngineData * vulkanEngineData):
	StoreScene(vulkanEngineData) {
}

void Store::StoreSceneNative::store(
	const VulkanEngineApplication::VulkanData * vulkanData, 
	const std::shared_ptr<Draw::Model>& model,
	const std::string &fileName) {
	std::ofstream scene(fileName, std::ios::binary | std::ios::out | std::ios::trunc);

	storeMaterials(scene, model);
	storeBuffers(vulkanData, scene, model);
	storeDrawData(scene, model);
	scene.write(reinterpret_cast<const char *>(&model->mWorld), sizeof(model->mWorld));

	scene.close();
}

void Store::StoreSceneNative::storeMaterials(std::ofstream & scene, const std::shared_ptr<Draw::Model>& model) {
	std::string textureName = "";
	uint32_t textureNameLength = 0;
	int32_t textureID = -1;
	//uint32_t textureCount = 0;

	uint32_t materialCount = static_cast<decltype(materialCount)>(model->materials.size());
	scene.write(reinterpret_cast<const char *>(&materialCount), sizeof(materialCount));

	std::queue<int32_t> qTextureIDs;
	for (const auto &mat: model->materials) {
		scene.write(reinterpret_cast<const char *>(&mat.diffuseColor), sizeof(mat.diffuseColor));
		scene.write(reinterpret_cast<const char *>(&mat.emissiveColor), sizeof(mat.emissiveColor));
		scene.write(reinterpret_cast<const char *>(&mat.specularColor), sizeof(mat.specularColor));

		qTextureIDs.push(mat.diffuseTextureID);
		qTextureIDs.push(mat.normalTextureID);
		qTextureIDs.push(mat.reflectionTextureID);
		qTextureIDs.push(mat.specularTextureID);

		//textureCount = static_cast<decltype(textureCount)>(qTextureIDs.size());
		//scene.write(reinterpret_cast<const char *>(&textureCount), sizeof(textureCount));

		while(!qTextureIDs.empty()) {
			textureID = qTextureIDs.front();
			textureName = "";
			textureNameLength = 0;
			if (textureID != -1) {
				textureName = vulkanEngineData->imageContainer->getNameByKey(textureID);
				textureNameLength = static_cast<decltype(textureNameLength)>(textureName.length());
			}
			scene.write(reinterpret_cast<const char *>(&textureNameLength), sizeof(textureNameLength));
			if (textureNameLength > 0) {
				scene.write(textureName.c_str(), textureNameLength);
			}
			qTextureIDs.pop();
		}
	}
}

void Store::StoreSceneNative::storeDrawData(std::ofstream & scene, const std::shared_ptr<Draw::Model>& model) {
	size_t sceneNameLength = model->drawData->drawDataName.length();
	scene.write(reinterpret_cast<const char *>(&sceneNameLength), sizeof(sceneNameLength));
	scene.write(model->drawData->drawDataName.c_str(), sceneNameLength);

	storeDrawDataVector(scene, model->drawData->firstIndex);
	storeDrawDataVector(scene, model->drawData->indexCount);
	storeDrawDataVector(scene, model->drawData->materialIdx);
	storeDrawDataVector(scene, model->drawData->vertexOffset);
}

void Store::StoreSceneNative::storeBuffers(const VulkanEngineApplication::VulkanData * vulkanData, std::ofstream & scene, VkBuffer buffer, VkDeviceSize bufferSize) {
	VkCommandBuffer commandBuffer = VulkanInitialize::beginSingleTimeCommand(vulkanData, vulkanData->commandPool);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	VulkanInitialize::createBuffer(vulkanData, bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingMemory);
	
	VkBufferCopy copyRegion = {};
	copyRegion.dstOffset = 0;
	copyRegion.srcOffset = 0;
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(commandBuffer, buffer, stagingBuffer, 1, &copyRegion);

	VulkanInitialize::endSingleTimeCommand(vulkanData, vulkanData->commandPool, commandBuffer);

	void *stagingBufferData;
	vkMapMemory(vulkanData->device, stagingMemory, 0, bufferSize, 0, &stagingBufferData);
	scene.write(reinterpret_cast<const char *>(&bufferSize), sizeof(VkDeviceSize));
	scene.write(reinterpret_cast<const char *>(stagingBufferData), bufferSize);
	vkUnmapMemory(vulkanData->device, stagingMemory);

	vkDestroyBuffer(vulkanData->device, stagingBuffer, nullptr);
	vkFreeMemory(vulkanData->device, stagingMemory, nullptr);
}

void Store::StoreSceneNative::storeBuffers(const VulkanEngineApplication::VulkanData * vulkanData, std::ofstream & scene, const std::shared_ptr<Draw::Model>& model) {
	storeBuffers(vulkanData, scene, model->buffer->vertexBuffer, model->buffer->vertexBufferSize);
	storeBuffers(vulkanData, scene, model->buffer->indexBuffer, model->buffer->indexBufferSize);
}
#endif