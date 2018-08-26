#include "MaterialLoader.h"

LoadManager::MaterialLoader::MaterialLoader(ImageManager::ImageContainer *imageContainer): 
	imageContainer(imageContainer) { }

const std::vector<Draw::Material>& LoadManager::MaterialLoader::getMaterials(void) const {
	return material;
}

void LoadManager::MaterialLoader::loadMaterialColors(const aiMaterial *pMaterial, uint32_t materialIdx) {
	aiColor4D color;
	aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_SPECULAR, &color);
	material[materialIdx].specularColor = glm::vec4(color.r, color.g, color.b, color.a);

	aiGetMaterialColor(pMaterial, AI_MATKEY_SHININESS, &color);
	material[materialIdx].specularColor.a = color.r * 0.25f;

	/*
	aiGetMaterialColor(pMaterial, AI_MATKEY_REFLECTIVITY, &mcol);//величина отражения света

	aiGetMaterialColor(pMaterial, AI_MATKEY_REFRACTI, &mcol);//величина преломления света

	aiGetMaterialColor(pMaterial, AI_MATKEY_OPACITY, &mcol);//величина прозрачности материала
	*/

	aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &color);
	material[materialIdx].diffuseColor = glm::vec4(color.r, color.g, color.b, color.a);

	aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_AMBIENT, &color);
	material[materialIdx].diffuseColor.a = pow((color.r / material[materialIdx].diffuseColor.r) * 0.8f, 2.5f);//а почему бы и нет :)

	aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_EMISSIVE, &color);
	material[materialIdx].emissiveColor = glm::vec4(color.r, color.g, color.b, color.a);
}

void LoadManager::MaterialLoader::loadMaterialTextures(const aiMaterial *pMaterial, uint32_t materialIdx, std::string sourcePath) {
	aiString Path;
	std::string texturePath;

	if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path,
			NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			texturePath = sourcePath + Path.data;
			imageContainer->addImage(texturePath);
			material[materialIdx].diffuseTextureID = imageContainer->getKeyByName(texturePath);
		}
	}

	if (pMaterial->GetTextureCount(aiTextureType_NORMALS) > 0) {
		if (pMaterial->GetTexture(aiTextureType_NORMALS, 0, &Path,
			NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			texturePath = sourcePath + Path.data;
			imageContainer->addImage(texturePath);
			material[materialIdx].normalTextureID = imageContainer->getKeyByName(texturePath);
		}
	}

	if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0) {
		if (pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &Path,
			NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			texturePath = sourcePath + Path.data;
			imageContainer->addImage(texturePath);
			material[materialIdx].specularTextureID = imageContainer->getKeyByName(texturePath);
		}
	}

	if (pMaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
		if (pMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &Path,
			NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			texturePath = sourcePath + Path.data;
			imageContainer->addImage(texturePath);
			material[materialIdx].reflectionTextureID = imageContainer->getKeyByName(texturePath);
		}
	}
}

void LoadManager::MaterialLoader::addMaterials(const aiScene * pScene, std::string sourcePath) {
	material.resize(pScene->mNumMaterials);

	for (uint32_t i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial *pMaterial = pScene->mMaterials[i];
		loadMaterialColors(pMaterial, i);
		loadMaterialTextures(pMaterial, i, sourcePath);
	}
}
