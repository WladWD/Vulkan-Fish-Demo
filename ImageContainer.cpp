#include "ImageContainer.h"


ImageManager::ImageContainer::ImageContainer(const Asset::AssetLoader * assetLoader, 
	const VulkanEngineApplication::VulkanData * vulkanData): imageLoader(nullptr), vulkanData(vulkanData) {
	imageLoader = new ImageLoader(assetLoader, vulkanData);
}

ImageManager::ImageContainer::~ImageContainer() {
	if (imageLoader) {
		delete imageLoader;
	}

	for (auto image: imageDatas) {
		vkDestroyImageView(vulkanData->device, image.imageView, nullptr);
		vkDestroyImage(vulkanData->device, image.image, nullptr);
		vkFreeMemory(vulkanData->device, image.imageMemory, nullptr);
	}

	imageNames.clear();
	imageKeys.clear();
	imageDatas.clear();
}

void ImageManager::ImageContainer::addImage(const std::string &imageName, bool useMipMap) {
	if (!imageKeys.count(imageName)) {
		imageKeys[imageName] = static_cast<uint32_t>(imageDatas.size());
		imageNames.push_back(imageName);
		imageDatas.push_back(imageLoader->loadImage(imageName.c_str(), useMipMap));
	}
}

std::string ImageManager::ImageContainer::getNameByKey(uint32_t key) const {
	if (key < 0 || key >= imageNames.size()) {
		throw std::runtime_error("[Error] undefined key");
	}
	return imageNames[key];
}

uint32_t ImageManager::ImageContainer::getKeyByName(const std::string &imageName) const {
	if (imageKeys.count(imageName) == 0) {
		throw std::runtime_error("[Error] undefined key");
	}

	return imageKeys[imageName];
}

const ImageManager::ImageData &ImageManager::ImageContainer::getImageDataByKey(uint32_t key) const {
	if (key < 0 || key >= imageNames.size()) {
		throw std::runtime_error("[Error] undefined key");
	}

	return imageDatas[key];
}

const ImageManager::ImageData &ImageManager::ImageContainer::getImageDataByName(const std::string &imageName) const {
	return imageDatas[getKeyByName(imageName)];
}

const ImageManager::ImageData & ImageManager::ImageContainer::operator[](uint32_t key) const {
	if (key < 0 || key >= imageNames.size()) {
		throw std::runtime_error("[Error] undefined key");
	}

	return imageDatas[key];
}

const ImageManager::ImageData & ImageManager::ImageContainer::operator[](const std::string & imageName) const {
	return imageDatas[getKeyByName(imageName)];
}
