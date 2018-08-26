#include "ImageLoader.h"
#include <unordered_map>

#pragma once
namespace ImageManager {
	class ImageContainer {
		const VulkanEngineApplication::VulkanData * vulkanData;
		ImageLoader *imageLoader;

		std::vector<ImageManager::ImageData> imageDatas;
		std::vector<std::string> imageNames;
		mutable std::unordered_map<std::string, uint32_t> imageKeys;

	public:
		ImageContainer(const Asset::AssetLoader * assetLoader, const VulkanEngineApplication::VulkanData * vulkanData);
		~ImageContainer();

		void addImage(const std::string &imageName, bool useMipMap = true);
		//eraseImageByKey
		//eraseImageByName

		std::string getNameByKey(uint32_t key) const;
		uint32_t getKeyByName(const std::string &imageName) const;

		const ImageManager::ImageData &getImageDataByKey(uint32_t key) const;
		const ImageManager::ImageData &getImageDataByName(const std::string &imageName) const;

		const ImageManager::ImageData &operator[](uint32_t key) const;
		const ImageManager::ImageData &operator[](const std::string &imageName) const;
	};
}
