#include "AssetLoader.h"

#pragma once
namespace Asset {
	class AssetReader {
	protected:
		uint8_t *data = nullptr;
		int64_t dataSize = 0;
		int64_t cursorPos = 0;

	public:
		AssetReader(const AssetLoader *assetLoader, std::string assetName);
		virtual ~AssetReader();

		virtual void read(void *data, int64_t dataSize);
	};
}

