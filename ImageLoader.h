#include "AssetLoader.h"
#include "ImageData.h"
#include "VulkanCreater.h"

#pragma once
namespace ImageManager {
	class ImageLoader {
		//typedef void (ImageManager::ImageLoader::*LoadImageFunction)(const gli::texture &texture, ImageManager::ImageData &imageData);
		//LoadImageFunction loadFunction[gli::target::TARGET_LAST + 1];
		//loadFunction[gli::target::TARGET_2D] = &ImageLoader::loadImage;
		//(this->*loadFunction[texture.target()])(texture, loadImageData);

		const VulkanEngineApplication::VulkanData * vulkanData;
		const Asset::AssetLoader * assetLoader;

		void transitionLayoutToSharedRead(ImageManager::ImageData &imageData);
		void generateMipMapLevels(ImageManager::ImageData &imageData);
		void generateMipMapLevels(gli::texture &texture, gli::filter Minification = gli::filter::FILTER_LINEAR);
		VkFormat selectImageFormat(gli::format imageFormat);
		VkImageType selectImageType(const gli::texture &texture);
		VkImageViewType selectImageViewType(const gli::texture &texture);
		void loadImage(const gli::texture &texture, ImageManager::ImageData &imageData, bool useMipMap);
	public:
		ImageLoader(const Asset::AssetLoader * assetLoader, const VulkanEngineApplication::VulkanData * vulkanData);
		~ImageLoader();

		ImageManager::ImageData loadImage(const char *imageName, bool useMipMap = true);
	};
}

