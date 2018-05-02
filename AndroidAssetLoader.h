#include "AssetLoader.h"
#include <cstdlib>

#pragma once
namespace Asset
{
	class AndroidAssetLoader : public AssetLoader
	{
		AAssetManager *pAssetManager;
	public:
		AndroidAssetLoader(AAssetManager *pAssetManager);
		~AndroidAssetLoader() = default;

		int32_t loadAssetByName(std::string assetName, uint8_t **data, int64_t &dataSize) const;
	};
}

