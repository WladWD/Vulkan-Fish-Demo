#include "StoreScene.h"

#pragma once
namespace Store {
	class StoreSceneNative: public StoreScene {
		void storeMaterials(std::ofstream &scene, const std::shared_ptr<Draw::Model>& model);
		void storeBuffers(std::ofstream &scene, const std::shared_ptr<Draw::Model>& model);
	public:
		StoreSceneNative(const VulkanEngineApplication::VulkanEngineData *vulkanEngineData);
		virtual ~StoreSceneNative() override = default;

		virtual void store(const std::shared_ptr<Draw::Model> &model, const std::string &fileName) override;
	};
}

