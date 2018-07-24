#include "ImageLoader.h"

ImageManager::ImageLoader::ImageLoader(const Asset::AssetLoader * assetLoader,
	const VulkanEngineApplication::VulkanData * vulkanData): assetLoader(assetLoader), vulkanData(vulkanData) {
}

ImageManager::ImageLoader::~ImageLoader() {
}

void ImageManager::ImageLoader::transitionLayoutToSharedRead(ImageManager::ImageData & imageData) {
	VkCommandBuffer commandBuffer = VulkanInitialize::beginSingleTimeCommand(vulkanData, vulkanData->commandPool);

	VulkanInitialize::transitionImageLayout(
		imageData.image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT,
		commandBuffer);

	VulkanInitialize::endSingleTimeCommand(vulkanData, vulkanData->commandPool, commandBuffer);
}

void ImageManager::ImageLoader::generateMipMapLevels(ImageManager::ImageData &imageData) {
	VkCommandBuffer commandBuffer = VulkanInitialize::beginSingleTimeCommand(vulkanData, vulkanData->commandPool);
	VkImageMemoryBarrier imageMemmoryBarrier = {};
	imageMemmoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemmoryBarrier.pNext = nullptr;
	imageMemmoryBarrier.image = imageData.image;
	imageMemmoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemmoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemmoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemmoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemmoryBarrier.subresourceRange.layerCount = imageData.layerCount;
	imageMemmoryBarrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = imageData.imageExtent.width;
	int32_t mipHeight = imageData.imageExtent.height;
	int32_t mipDepth = imageData.imageExtent.depth;

	for (uint32_t i = 1; i < imageData.levelCount; ++i) {
		
		imageMemmoryBarrier.subresourceRange.baseMipLevel = i - 1;
		imageMemmoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemmoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		imageMemmoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		imageMemmoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr, 0, nullptr,
			1, &imageMemmoryBarrier);

		VkImageBlit imageBlit = {};
		imageBlit.srcOffsets[0] = { 0, 0, 0 };
		imageBlit.srcOffsets[1] = { mipWidth, mipHeight, mipDepth };
		imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.srcSubresource.baseArrayLayer = 0;
		imageBlit.srcSubresource.layerCount = imageData.layerCount;
		imageBlit.srcSubresource.mipLevel = imageMemmoryBarrier.subresourceRange.baseMipLevel;

		imageBlit.dstOffsets[0] = { 0, 0, 0 };
		imageBlit.dstOffsets[1] = { std::max(1, mipWidth >> 1),  std::max(1, mipHeight >> 1),  std::max(1, mipDepth >> 1) };
		imageBlit.dstSubresource.aspectMask = imageBlit.srcSubresource.aspectMask;
		imageBlit.dstSubresource.baseArrayLayer = imageBlit.srcSubresource.baseArrayLayer;
		imageBlit.dstSubresource.layerCount = imageBlit.srcSubresource.layerCount;
		imageBlit.dstSubresource.mipLevel = i;

		vkCmdBlitImage(commandBuffer, 
			imageData.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			imageData.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &imageBlit,
			VK_FILTER_LINEAR);

		imageMemmoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		imageMemmoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageMemmoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		imageMemmoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &imageMemmoryBarrier);

		if (mipWidth > 1) mipWidth >>= 1;
		if (mipHeight > 1) mipHeight >>= 1;
		if (mipDepth > 1) mipDepth >>= 1;
	}

	imageMemmoryBarrier.subresourceRange.baseMipLevel = imageData.levelCount - 1;
	imageMemmoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageMemmoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageMemmoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageMemmoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,//VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		0, nullptr,
		0, nullptr,
		1, &imageMemmoryBarrier);

	VulkanInitialize::endSingleTimeCommand(vulkanData, vulkanData->commandPool, commandBuffer);
}

void ImageManager::ImageLoader::generateMipMapLevels(gli::texture &texture, gli::filter Minification) {
	switch (texture.target()) {
	case gli::target::TARGET_1D: texture = gli::generate_mipmaps(gli::texture1d(texture), Minification); break;
	case gli::target::TARGET_1D_ARRAY: texture = gli::generate_mipmaps(gli::texture1d_array(texture), Minification); break;

	case gli::target::TARGET_2D: texture = gli::generate_mipmaps(gli::texture2d(texture), Minification); break;
	case gli::target::TARGET_2D_ARRAY: texture = gli::generate_mipmaps(gli::texture2d_array(texture), Minification); break;

	case gli::target::TARGET_CUBE: texture = gli::generate_mipmaps(gli::texture_cube(texture), Minification); break;
	case gli::target::TARGET_CUBE_ARRAY: texture = gli::generate_mipmaps(gli::texture_cube_array(texture), Minification); break;

	case gli::target::TARGET_3D: texture = gli::generate_mipmaps(gli::texture3d(texture), Minification); break;

	default: throw std::runtime_error("[Error] undefined image type");//undefined
	}
}

VkFormat ImageManager::ImageLoader::selectImageFormat(gli::format imageFormat) {
	switch (imageFormat) {
	case gli::format::FORMAT_BGRA8_UNORM_PACK8:
	case gli::format::FORMAT_BGR8_UNORM_PACK32: return VK_FORMAT_B8G8R8A8_UNORM;

	default: throw std::runtime_error("[Error] undefined image format");
	}
}

VkImageType ImageManager::ImageLoader::selectImageType(const gli::texture &texture) {
	switch (texture.target()) {
		case gli::target::TARGET_1D:
		case gli::target::TARGET_1D_ARRAY:
			return VK_IMAGE_TYPE_1D;

		case gli::target::TARGET_2D:
		case gli::target::TARGET_2D_ARRAY:
		case gli::target::TARGET_CUBE:
		case gli::target::TARGET_CUBE_ARRAY:
			return VK_IMAGE_TYPE_2D;

		case gli::target::TARGET_3D:
			return VK_IMAGE_TYPE_3D;

		default:
			return VK_IMAGE_TYPE_MAX_ENUM;//undefined
	}
}

VkImageViewType ImageManager::ImageLoader::selectImageViewType(const gli::texture &texture) {
	switch (texture.target()) {
		case gli::target::TARGET_1D: return VK_IMAGE_VIEW_TYPE_1D;
		case gli::target::TARGET_1D_ARRAY: return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

		case gli::target::TARGET_2D: return VK_IMAGE_VIEW_TYPE_2D;
		case gli::target::TARGET_2D_ARRAY: return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

		case gli::target::TARGET_CUBE: return VK_IMAGE_VIEW_TYPE_CUBE;
		case gli::target::TARGET_CUBE_ARRAY: return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

		case gli::target::TARGET_3D: return VK_IMAGE_VIEW_TYPE_3D;

		default:
			return VK_IMAGE_VIEW_TYPE_MAX_ENUM;//undefined
	}
}

void ImageManager::ImageLoader::loadImage(const gli::texture &texture, ImageManager::ImageData &imageData, bool useMipMap) {
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.arrayLayers = static_cast<uint32_t>(texture.layers());
	imageInfo.flags = 0;
	imageInfo.imageType = selectImageType(texture);
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.mipLevels = static_cast<uint32_t>(texture.levels());
	imageInfo.pNext = nullptr;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	imageInfo.extent = imageData.imageExtent; 
	imageInfo.format = imageData.imageFormat;

	bool needGenerateMipLevel = false;
	if (useMipMap && static_cast<uint32_t>(texture.levels()) == 1) {
		imageInfo.mipLevels = static_cast<uint32_t>(
			std::floor(
				std::log2(
					std::max(
						imageInfo.extent.depth, std::max(imageInfo.extent.width, imageInfo.extent.height))))) + 1;
		needGenerateMipLevel = true;
	}

	imageData.layerCount = imageInfo.arrayLayers;
	imageData.levelCount = imageInfo.mipLevels;
	VulkanInitialize::createImage(vulkanData, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, &imageInfo, imageData.image, imageData.imageMemory);
	VulkanInitialize::initializeImage(vulkanData,
		vulkanData->commandPool,
		texture.data(),
		static_cast<VkDeviceSize>(texture.size()),
		imageInfo.extent,
		imageInfo.format,
		VK_IMAGE_ASPECT_COLOR_BIT,
		imageInfo.arrayLayers,
		imageData.image);

	if (needGenerateMipLevel) {
		generateMipMapLevels(imageData);
	}
	else {
		transitionLayoutToSharedRead(imageData);
	}

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = imageData.image;
	viewInfo.viewType = selectImageViewType(texture);
	viewInfo.format = imageInfo.format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseArrayLayer = static_cast<uint32_t>(texture.base_layer());
	viewInfo.subresourceRange.baseMipLevel = static_cast<uint32_t>(texture.base_level());
	viewInfo.subresourceRange.layerCount = imageInfo.arrayLayers;
	viewInfo.subresourceRange.levelCount = imageInfo.mipLevels;

	if (vkCreateImageView(vulkanData->device, &viewInfo, nullptr, &imageData.imageView) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed create image view");
	}
}

ImageManager::ImageData ImageManager::ImageLoader::loadImage(const char * imageName, bool useMipMap) {
	ImageManager::ImageData loadImageData;

	uint8_t *data = nullptr;
	int64_t dataSize = 0;

	assetLoader->loadAssetByName(std::string(imageName), &data, dataSize);
	gli::texture texture = gli::load_dds(reinterpret_cast<char *>(data), static_cast<size_t>(dataSize));

	/*if (useMipMap) {
		auto x = gli::convert(gli::texture2d(texture), gli::FORMAT_RGB32_SFLOAT_PACK32);
		gli::texture2d texture2dD = gli::generate_mipmaps(gli::texture2d(x), gli::filter::FILTER_LINEAR);
		generateMipMapLevels(texture, gli::filter::FILTER_LINEAR);
	}*/

	loadImageData.imageExtent = { static_cast<uint32_t>(texture.extent().x), static_cast<uint32_t>(texture.extent().y), static_cast<uint32_t>(texture.extent().z) };
	loadImageData.imageFormat = selectImageFormat(texture.format());
	loadImage(texture, loadImageData, useMipMap);

	if (data != nullptr) {
		delete[] data;
	}

	return loadImageData;
}

//GLI code samples
//https://github.com/g-truc/gli/blob/master/manual.md
