#include "VulkanEngineData.h"
#include "LoadDiffuseMesh.h"

#pragma once
namespace LoadManager {
	class LoadModel {
	protected:
		const VulkanEngineApplication::VulkanData * vulkanData;
		const VulkanEngineApplication::VulkanEngineData *vulkanEngineData;
		std::shared_ptr<ImageManager::ImageLoader> imageLoader;
		VkBuffer vertexBuffer, indexBuffer;
		VkDeviceMemory vertexBufferMemory, indexBufferMemory;
		std::shared_ptr<Draw::Model> scene;

		virtual VkDeviceSize packVertexBuffer(void) = 0;
		virtual VkDeviceSize packIndexBuffer(void) = 0;
	public:
		LoadModel(const Asset::AssetLoader * assetLoader,
			const VulkanEngineApplication::VulkanData * vulkanData,
			const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
			const std::shared_ptr<Draw::Model> &scene);
		virtual ~LoadModel() = default;

		virtual void addModel(const std::string &model) = 0;
		virtual void packScene(void);
		virtual const std::shared_ptr<Draw::Model> &getLoadedScene(void);
	};
}
