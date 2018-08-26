#include "LoadDiffuseMesh.h"
#include "VulkanEngineData.h"

#ifdef USE_ASSIMP
#pragma once
namespace LoadManager {
	class LoadModelAssimp {
		const VulkanEngineApplication::VulkanData * vulkanData;

		std::unique_ptr<LoadManager::MaterialLoader> materialLoader;
		std::shared_ptr<ImageManager::ImageLoader> imageLoader;
		std::vector<std::unique_ptr<LoadMesh>> loadMesh;

		VkBuffer vertexBuffer, indexBuffer;
		VkDeviceMemory vertexBufferMemory, indexBufferMemory;

		std::shared_ptr<Draw::Model> scene;

		VkDeviceSize packVertexBuffer(void);
		VkDeviceSize packIndexBuffer(void);
	public:
		LoadModelAssimp(const Asset::AssetLoader * assetLoader,
			const VulkanEngineApplication::VulkanData * vulkanData,
			const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
			const std::shared_ptr<Draw::Model> &scene);
		~LoadModelAssimp() = default;

		void addModel(std::string model);
		void packScene(void);
		const std::shared_ptr<Draw::Model> &getLoadedScene(void);
	};
}
#endif


