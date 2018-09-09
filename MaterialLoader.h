#include "MaterialStruct.h"
#include "ImageContainer.h"
#include <map>

#ifdef USE_ASSIMP
#pragma once
namespace LoadManager {
	class MaterialLoader {
		ImageManager::ImageContainer *imageContainer;
		std::vector<Draw::Material> material;

		void loadMaterialColors(const aiMaterial *pMaterial, uint32_t materialIdx);
		void loadMaterialTextures(const aiMaterial *pMaterial, uint32_t materialIdx, std::string sourcePath);
	public:
		MaterialLoader(ImageManager::ImageContainer *imageContainer);
		~MaterialLoader() = default;

		const std::vector<Draw::Material> &getMaterials(void) const;
		void addMaterials(const aiScene* pScene, std::string sourcePath);
	};
}
#endif

