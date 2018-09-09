#include "LoadMeshBase.h"

#ifdef USE_ASSIMP
#pragma once
namespace LoadManager {
	class LoadMeshAssimp: public LoadMeshBase {
	public:
		LoadMeshAssimp(const std::shared_ptr<ImageManager::ImageLoader> &imageLoader,
			const std::shared_ptr<Draw::Model> &scene,
			Draw::DrawDataTypes loadDrawType);
		virtual ~LoadMeshAssimp() = default;

		virtual void addModel(const aiScene *pScene) = 0;
	};
}
#endif

