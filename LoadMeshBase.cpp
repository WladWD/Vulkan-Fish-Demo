#include "LoadMeshBase.h"

uint32_t LoadManager::LoadMeshBase::startIndex = 0;
uint32_t LoadManager::LoadMeshBase::startVertex = 0;

LoadManager::LoadMeshBase::LoadMeshBase(
	const std::shared_ptr<ImageManager::ImageLoader>& imageLoader,
	const std::shared_ptr<Draw::Model>& scene,
	Draw::DrawDataTypes loadDrawType):
	imageLoader(imageLoader), scene(scene),
	loadDrawType(static_cast<uint32_t>(loadDrawType)) {
}
