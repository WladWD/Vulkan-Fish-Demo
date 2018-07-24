#include "InitStruct.h"
#include "ImageData.h"
#include "VulkanCreater.h"

#pragma once
namespace Draw {
	class Texture {
		const VulkanEngineApplication::VulkanData * vulkanData;
		ImageManager::ImageData imageData;
		VkImageLayout imageLayout;
		VkImageAspectFlags imageAspect;

		void createImage(const VkImageCreateInfo &imageInfo);
		void createImageView(const VkImageViewCreateInfo &viewInfo);
	public:
		Texture(
			const VulkanEngineApplication::VulkanData * vulkanData,
			const VkImageCreateInfo &imageInfo,
			const VkImageViewCreateInfo &viewInfo);
		~Texture();

		void generateMipLevels(void) = delete;
		void transitionLayout(VkImageLayout targetImageLayout,
			VkAccessFlags srcAccess, VkAccessFlags dstAccess,
			VkPipelineStageFlags srcStageMack, VkPipelineStageFlags dstStageMack);
		const ImageManager::ImageData &getImageData(void);

		static uint32_t getMipCount(const VkExtent3D &extent);
	};

	//TODO generateMipLevels
}

