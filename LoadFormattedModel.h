#include "LoadModel.h"
#include "LoadMeshBase.h"
#include <map>

#pragma once
namespace LoadManager {
	class LoadFormattedModel: public LoadModel {
	protected:
		std::map<Draw::DrawDataTypes, std::unique_ptr<LoadMeshBase>> loadMesh;

		virtual VkDeviceSize packVertexBuffer(void) override;
		virtual VkDeviceSize packIndexBuffer(void) override;
	public:
		LoadFormattedModel(const Asset::AssetLoader * assetLoader,
			const VulkanEngineApplication::VulkanData * vulkanData,
			const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
			const std::shared_ptr<Draw::Model> &scene);
		virtual ~LoadFormattedModel() = default;
	};
}

//virtual VkDeviceSize packVertexBuffer(void) = 0;
//virtual VkDeviceSize packIndexBuffer(void) = 0;