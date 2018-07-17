#include "LoadDiffuseMesh.h"

#ifdef USE_ASSIMP
LoadManager::LoadDiffuseMesh::LoadDiffuseMesh(
	const std::shared_ptr<ImageManager::ImageLoader>& imageLoader,
	const std::shared_ptr<Draw::Model>& scene):
	LoadMesh(imageLoader, scene, Draw::DrawDataTypes::DrawDataDiffuse) {

}

void LoadManager::LoadDiffuseMesh::addVertexToBuffer(const aiMesh * paiMesh) {
	Draw::Vertex vertex;

	for (register uint32_t i = 0; i < paiMesh->mNumVertices; i++) {
		aiVector3D vert_pos = paiMesh->mVertices[i];
		aiVector3D vert_norm = paiMesh->mNormals[i];
		aiVector3D vert_tex = paiMesh->mTextureCoords[0][i];

		vertex.position = glm::vec3(vert_pos.x, vert_pos.y, vert_pos.z);
		vertex.normal = glm::normalize(glm::vec3(vert_norm.x, vert_norm.y, vert_norm.z));
		vertex.texCoord = glm::vec2(vert_tex.x, vert_tex.y);

		vertexBuffer.push_back(vertex);
	}
}

void LoadManager::LoadDiffuseMesh::addIndexToBuffer(const aiMesh *paiMesh) {
	for (register uint32_t i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& face = paiMesh->mFaces[i];
		assert(face.mNumIndices == 3);

		for (uint32_t j = 0; j < face.mNumIndices; ++j) {
			indexBuffer.push_back(face.mIndices[j]);
		}
	}
}

void LoadManager::LoadDiffuseMesh::addModel(const aiScene * pScene) {
	//has tex coord, has diffuse texture
	for (uint32_t i = 0; i < pScene->mNumMeshes; i++) {
		if (pScene->mMeshes[i]->HasTextureCoords(0) &&
			scene->materials[pScene->mMeshes[i]->mMaterialIndex].diffuseTextureID) {
			const aiMesh *paiMesh = pScene->mMeshes[i];

			scene->drawData[loadDrawType].drawDataName = paiMesh->mName.C_Str();
			scene->drawData[loadDrawType].materialIdx.push_back(paiMesh->mMaterialIndex);
			scene->drawData[loadDrawType].firstIndex.push_back(LoadManager::LoadMesh::startIndex);
			scene->drawData[loadDrawType].vertexOffset.push_back(LoadManager::LoadMesh::startVertex);
			scene->drawData[loadDrawType].indexCount.push_back(paiMesh->mNumFaces * 3);

			addVertexToBuffer(paiMesh);
			addIndexToBuffer(paiMesh);

			LoadManager::LoadMesh::startVertex += paiMesh->mNumVertices;
			LoadManager::LoadMesh::startIndex += paiMesh->mNumFaces * 3;
		}
	}
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