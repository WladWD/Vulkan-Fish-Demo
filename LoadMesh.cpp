#include "LoadMesh.h"

#ifdef USE_ASSIMP
LoadManager::LoadMesh::LoadMesh(
	const std::shared_ptr<ImageManager::ImageLoader>& imageLoader,
	const std::shared_ptr<Draw::Model>& scene,
	Draw::DrawDataTypes loadDrawType):
	imageLoader(imageLoader), scene(scene),
	loadDrawType(static_cast<uint32_t>(loadDrawType)) {
}
#endif