#include "AndroidAssetLoader.h"


Asset::AndroidAssetLoader::AndroidAssetLoader(AAssetManager *pAssetManager) : pAssetManager(pAssetManager) {

}

int32_t Asset::AndroidAssetLoader::loadAssetByName(std::string assetName, uint8_t ** data, int64_t &dataSize) const {
    for(char &c : assetName) {
        if(c == '\\') c = '/';
    }
    AAsset  *file = AAssetManager_open(pAssetManager, assetName.c_str(), AASSET_MODE_BUFFER);//"Resources\\Images\\initialize.dds"
   // auto dir = AAssetManager_openDir(pAssetManager, "Resources\\Images");

    dataSize = AAsset_getLength(file);
    *data = new uint8_t[dataSize];
    AAsset_read(file, static_cast<void *>(*data), dataSize);
    AAsset_close(file);

    return 0;//result code
}
