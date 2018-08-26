#include "VulkanEngineData.h"
#include "DrawData.h"

#pragma once
namespace Store {
	class StoreScene {
	protected: 
		const VulkanEngineApplication::VulkanEngineData *vulkanEngineData;
	public:
		StoreScene(const VulkanEngineApplication::VulkanEngineData *vulkanEngineData);
		virtual ~StoreScene() = default;

		virtual void store(const std::shared_ptr<Draw::Model> &model, const std::string &fileName) = 0;
	};
}

