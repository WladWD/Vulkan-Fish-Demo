#include "LoadMesh.h"
#include "VertexStruct.h"

#ifdef USE_ASSIMP
#pragma once
namespace LoadManager {
	class LoadDiffuseMesh: public LoadMesh {

		std::vector<Draw::Vertex> vertexBuffer;
		std::vector<uint32_t> indexBuffer;

		void addVertexToBuffer(const aiMesh *paiMesh);
		void addIndexToBuffer(const aiMesh *paiMesh);
	public:
		LoadDiffuseMesh(
			const std::shared_ptr<ImageManager::ImageLoader> &imageLoader,
			const std::shared_ptr<Draw::Model> &scene);
		~LoadDiffuseMesh() = default;

		virtual void addModel(const aiScene *pScene) override;

		virtual const void *getRawVertexData(void) override;
		virtual uint32_t getRawVertexDataSize(void) override;

		virtual const void *getRawIndexData(void) override;
		virtual uint32_t getRawIndexDataSize(void) override;
	};
}
#endif
