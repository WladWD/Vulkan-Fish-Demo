#include "LoadDiffuseMesh.h"

#ifdef USE_ASSIMP
LoadManager::LoadDiffuseMesh::LoadDiffuseMesh(
	const std::shared_ptr<ImageManager::ImageLoader>& imageLoader,
	const std::shared_ptr<Draw::Model>& scene):
	LoadMesh(imageLoader, scene, Draw::DrawDataTypes::DrawDataDiffuse) {

}

void LoadManager::LoadDiffuseMesh::addModel(const aiScene * pScene) {
	//has tex coord, has diffuse texture

}

const void * LoadManager::LoadDiffuseMesh::getRawVertexData(void) {
	return vertexBuffer.data();
}

uint32_t LoadManager::LoadDiffuseMesh::getRawVertexDataSize(void) {
	return static_cast<uint32_t>(vertexBuffer.size() * sizeof(Draw::Vertex));
}

const void * LoadManager::LoadDiffuseMesh::getRawIndexData(void) {
	return indexBuffer.data();
}

uint32_t LoadManager::LoadDiffuseMesh::getRawIndexDataSize(void) {
	return static_cast<uint32_t>(indexBuffer.size() * sizeof(uint16_t));
}
#endif