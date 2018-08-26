#include "StoreSceneNative.h"
#include <fstream>

Store::StoreSceneNative::StoreSceneNative(const VulkanEngineApplication::VulkanEngineData * vulkanEngineData):
	StoreScene(vulkanEngineData) {
}

void Store::StoreSceneNative::store(const std::shared_ptr<Draw::Model>& model, const std::string &fileName) {
	std::ofstream scene(fileName, std::ios::binary | std::ios::out | std::ios::trunc);

	storeMaterials(scene, model);

	model->buffer;//TODO write buffers
	model->drawData;//TODO write draw datas
	scene.write(reinterpret_cast<const char *>(&model->mWorld), sizeof(model->mWorld));

	scene.close();
}

void Store::StoreSceneNative::storeMaterials(std::ofstream & scene, const std::shared_ptr<Draw::Model>& model) {
	std::string textureName = "";
	uint32_t textureNameLength = 0;
	int32_t textureID = -1;

	uint32_t materialCount = static_cast<decltype(materialCount)>(model->materials.size());
	scene.write(reinterpret_cast<const char *>(&scene), sizeof(scene));

	std::queue<int32_t> qTextureIDs;
	for (const auto &mat: model->materials) {
		scene.write(reinterpret_cast<const char *>(&mat.diffuseColor), sizeof(mat.diffuseColor));
		scene.write(reinterpret_cast<const char *>(&mat.emissiveColor), sizeof(mat.emissiveColor));
		scene.write(reinterpret_cast<const char *>(&mat.specularColor), sizeof(mat.specularColor));

		qTextureIDs.push(mat.diffuseTextureID);
		qTextureIDs.push(mat.normalTextureID);
		qTextureIDs.push(mat.reflectionTextureID);
		qTextureIDs.push(mat.specularTextureID);

		while(!qTextureIDs.empty()) {
			textureID = qTextureIDs.front();
			textureName = "";
			if (textureID != -1) {
				textureName = vulkanEngineData->imageContainer->getNameByKey(textureID);
				textureNameLength = static_cast<decltype(textureNameLength)>(textureName.length());
			}
			scene.write(reinterpret_cast<const char *>(&textureNameLength), sizeof(textureNameLength));
			scene.write(textureName.c_str(), textureNameLength);
			qTextureIDs.pop();
		}
	}
}

void Store::StoreSceneNative::storeBuffers(std::ofstream & scene, const std::shared_ptr<Draw::Model>& model) {
	model->buffer;

	//transition need
	//create buffer
	//copy buffer
	//transition (from dst copy -> to host read, from dedevice read only)
}
