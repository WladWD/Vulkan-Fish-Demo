#include "LoadModelNative.h"

LoadManager::LoadModelNative::LoadModelNative(
	const Asset::AssetLoader * assetLoader,
	const VulkanEngineApplication::VulkanData * vulkanData,
	const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
	const std::shared_ptr<Draw::Model> &scene): LoadModel(assetLoader, vulkanData, vulkanEngineData, scene), 
	vertexBufferSize(0), indexBufferSize(0), assetLoader(assetLoader) { }

void LoadManager::LoadModelNative::addModel(const std::string &model) {
	//std::ifstream scene(model, std::ios::binary | std::ios::in);
	Asset::AssetReader scene(assetLoader, model);
	loadMaterials(scene);
	loadBuffers(scene);
	loadDrawData(scene);
	scene.read(reinterpret_cast<char *>(&this->scene->mWorld), sizeof(this->scene->mWorld));
}

VkDeviceSize LoadManager::LoadModelNative::loadBuffer(Asset::AssetReader &scene, VkBuffer &buffer,
	VkDeviceMemory &bufferMemory, VkBufferUsageFlags bufferUsage) {
	VkDeviceSize bufferSize = 0;
	void *bufferData = nullptr;

	scene.read(reinterpret_cast<char *>(&bufferSize), sizeof(decltype(bufferSize)));
	assert(bufferSize >= 0);

	bufferData = new uint8_t[bufferSize];
	scene.read(reinterpret_cast<char *>(bufferData), bufferSize);

	VulkanInitialize::createInitializeDeviceLocalBuffer(
		vulkanData,
		vulkanData->commandPool,
		bufferUsage,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferSize,
		bufferData,
		buffer,
		bufferMemory
	);

	delete[] bufferData;

	return bufferSize;
}

void LoadManager::LoadModelNative::loadBuffers(Asset::AssetReader &scene) {
	vertexBufferSize = loadBuffer(scene, vertexBuffer, vertexBufferMemory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	indexBufferSize = loadBuffer(scene, indexBuffer, indexBufferMemory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

void LoadManager::LoadModelNative::loadMaterials(Asset::AssetReader &scene) {
	std::queue<int32_t *> qTextureIDs;
	Draw::Material material;
	char *textureName;
	uint32_t textureNameLength = 0;

	uint32_t materialCount = 0;
	scene.read(reinterpret_cast<char *>(&materialCount), sizeof(materialCount));
	this->scene->materials.resize(materialCount);
	for (uint32_t i = 0; i < materialCount; ++i) {
		scene.read(reinterpret_cast<char *>(&material.diffuseColor), sizeof(material.diffuseColor));
		scene.read(reinterpret_cast<char *>(&material.emissiveColor), sizeof(material.emissiveColor));
		scene.read(reinterpret_cast<char *>(&material.specularColor), sizeof(material.specularColor));

		qTextureIDs.push(&material.diffuseTextureID);
		qTextureIDs.push(&material.normalTextureID);
		qTextureIDs.push(&material.reflectionTextureID);
		qTextureIDs.push(&material.specularTextureID);

		while (!qTextureIDs.empty()) {
			int32_t *textureID = qTextureIDs.front();

			scene.read(reinterpret_cast<char *>(&textureNameLength), sizeof(textureNameLength));
			textureName = new char[textureNameLength + 1];
			scene.read(textureName, textureNameLength);
			textureName[textureNameLength] = 0;
			if (textureNameLength) {
				vulkanEngineData->imageContainer->addImage(textureName);
				*textureID = vulkanEngineData->imageContainer->getKeyByName(textureName);
			}

			delete[] textureName;
			qTextureIDs.pop();
		}

		this->scene->materials[i] = material;
	}
}

void LoadManager::LoadModelNative::loadDrawData(Asset::AssetReader &scene) {
	size_t sceneNameLength = 0;
	std::vector<char> sceneName;
	scene.read(reinterpret_cast<char *>(&sceneNameLength), sizeof(sceneNameLength));
	sceneName.resize(sceneNameLength + 1);
	scene.read(sceneName.data(), sceneNameLength);
	sceneName[sceneNameLength] = 0;
	this->scene->drawData->drawDataName = sceneName.data();

	loadDrawDataVector(scene, this->scene->drawData->firstIndex);
	loadDrawDataVector(scene, this->scene->drawData->indexCount);
	loadDrawDataVector(scene, this->scene->drawData->materialIdx);
	loadDrawDataVector(scene, this->scene->drawData->vertexOffset);
}

VkDeviceSize LoadManager::LoadModelNative::packVertexBuffer(void) {
	return vertexBufferSize;
}

VkDeviceSize LoadManager::LoadModelNative::packIndexBuffer(void) {
	return indexBufferSize;
}


