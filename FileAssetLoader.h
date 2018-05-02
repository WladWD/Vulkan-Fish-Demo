#include "AssetLoader.h"
#include <cstdlib>

#pragma once
namespace Asset
{
	class FileAssetLoader : public AssetLoader
	{
	public:
		FileAssetLoader() = default;
		~FileAssetLoader() = default;

		int32_t loadAssetByName(std::string assetName, uint8_t **data, int64_t &dataSize) const;
	};
}

