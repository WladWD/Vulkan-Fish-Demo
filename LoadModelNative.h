#include "LoadModel.h" 
#include "AssetReader.h"

#pragma once
namespace LoadManager {
	class LoadModelNative: public LoadModel {
	private:
		const Asset::AssetLoader * assetLoader;

		VkDeviceSize loadBuffer(Asset::AssetReader &scene, VkBuffer &buffer,
			VkDeviceMemory &bufferMemory, VkBufferUsageFlags bufferUsage);
		void loadBuffers(Asset::AssetReader &scene);
		void loadMaterials(Asset::AssetReader &scene);
		void loadDrawData(Asset::AssetReader &scene);

		template<typename T>
		void loadDrawDataVector(Asset::AssetReader &scene, std::vector<T> &data) {
			auto dataSize = data.size();
			scene.read(reinterpret_cast<char *>(&dataSize), sizeof(decltype(dataSize)));
			data.resize(dataSize);
			scene.read(reinterpret_cast<char *>(data.data()), dataSize * sizeof(T));
		}

		VkDeviceSize vertexBufferSize;
		VkDeviceSize indexBufferSize;

		virtual VkDeviceSize packVertexBuffer(void) override;
		virtual VkDeviceSize packIndexBuffer(void) override;
	public:
		LoadModelNative(const Asset::AssetLoader * assetLoader,
			const VulkanEngineApplication::VulkanData * vulkanData,
			const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
			const std::shared_ptr<Draw::Model> &scene);
		virtual ~LoadModelNative() = default;

		virtual void addModel(const std::string &model) override;
	};
}

