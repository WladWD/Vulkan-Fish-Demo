#include "StoreScene.h"
#include <fstream>

#ifdef USE_ASSIMP
#pragma once
namespace Store {
	class StoreSceneNative: public StoreScene {
		void storeMaterials(std::ofstream &scene, const std::shared_ptr<Draw::Model>& model);

		template<typename T>
		void storeDrawDataVector(std::ofstream &scene, const std::vector<T> &data) {
			const auto &dataSize = data.size();
			scene.write(reinterpret_cast<const char *>(&dataSize), sizeof(decltype(dataSize)));
			scene.write(reinterpret_cast<const char *>(data.data()), data.size() * sizeof(T));
		}

		void storeDrawData(std::ofstream &scene, const std::shared_ptr<Draw::Model>& model);
		void storeBuffers(const VulkanEngineApplication::VulkanData * vulkanData, std::ofstream &scene, VkBuffer buffer, VkDeviceSize bufferSize);
		void storeBuffers(const VulkanEngineApplication::VulkanData * vulkanData, std::ofstream &scene, const std::shared_ptr<Draw::Model>& model);
	public:
		StoreSceneNative(const VulkanEngineApplication::VulkanEngineData *vulkanEngineData);
		virtual ~StoreSceneNative() override = default;

		virtual void store(
			const VulkanEngineApplication::VulkanData * vulkanData,
			const std::shared_ptr<Draw::Model> &model, 
			const std::string &fileName) override;
	};
}
#endif

