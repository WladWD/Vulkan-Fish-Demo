#include "ImageContainer.h"
#include "DrawData.h"
#include "MaterialLoader.h"

#pragma once
namespace LoadManager {
	class LoadMeshBase {

	protected:
		std::shared_ptr<ImageManager::ImageLoader> imageLoader;
		std::shared_ptr<Draw::Model> scene;

		uint32_t loadDrawType;

		static uint32_t startIndex;
		static uint32_t startVertex;

	public:
		LoadMeshBase(const std::shared_ptr<ImageManager::ImageLoader> &imageLoader,
			const std::shared_ptr<Draw::Model> &scene,
			Draw::DrawDataTypes loadDrawType);
		virtual ~LoadMeshBase() = default;

		//virtual void addModel(const aiScene *pScene) = 0;

		virtual const void *getRawVertexData(void) = 0;
		virtual uint32_t getRawVertexDataSize(void) = 0;

		virtual const void *getRawIndexData(void) = 0;
		virtual uint32_t getRawIndexDataSize(void) = 0;
	};
}
