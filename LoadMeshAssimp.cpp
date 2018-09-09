#include "LoadMeshAssimp.h"

#ifdef USE_ASSIMP
LoadManager::LoadMeshAssimp::LoadMeshAssimp(
	const std::shared_ptr<ImageManager::ImageLoader>& imageLoader,
	const std::shared_ptr<Draw::Model>& scene,
	Draw::DrawDataTypes loadDrawType):
	LoadMeshBase(imageLoader, scene, loadDrawType) { }
#endif