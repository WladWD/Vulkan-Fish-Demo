#include "FileAssetLoader.h"


int32_t Asset::FileAssetLoader::loadAssetByName(std::string assetName, uint8_t ** data, int64_t &dataSize) const {
	FILE *asset = fopen(assetName.c_str(), "rb");
	fseek(asset, 0, SEEK_END);
	dataSize = static_cast<int64_t>(ftell(asset));
	rewind(asset);
	*data = static_cast<uint8_t *>(malloc(static_cast<size_t>(dataSize)));
	fread(*data, sizeof(uint8_t), static_cast<size_t>(dataSize), asset);
	fclose(asset);

	return 0;//result code
}
