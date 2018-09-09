#include "AssetReader.h"

Asset::AssetReader::AssetReader(const AssetLoader * assetLoader, std::string assetName) {
	assetLoader->loadAssetByName(assetName, &data, dataSize);
}

Asset::AssetReader::~AssetReader() {
	delete[] data;
}

void Asset::AssetReader::read(void * data, int64_t dataSize) {
	assert(cursorPos + dataSize <= this->dataSize);
	memcpy(data, this->data + cursorPos, dataSize);
	cursorPos += dataSize;
}
