#include "VulkanHeader.h"

#pragma once
namespace Asset 
{
	class AssetLoader
	{
	public:
		AssetLoader() = default;
		virtual ~AssetLoader() = default;

		virtual int32_t loadAssetByName(std::string assetName, uint8_t **data, int64_t &dataSize) const = 0;
	};
}

