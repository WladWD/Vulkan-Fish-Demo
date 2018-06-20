#include "SamplersContainer.h"


Samplers::SamplersContainer::SamplersContainer(VkDevice device): device(device) {
	initializeMinMaxMagLinearUVWWrap();
}

Samplers::SamplersContainer::~SamplersContainer() {
	for (uint32_t i = 0; i < MaxMipLevels; ++i) {
		for (uint32_t j = 0; j < MaxMipLevels; ++j) {
			vkDestroySampler(device, minMaxMag_Linear_UVW_Wrap[i][j], nullptr);
		}
	}
}

const VkSampler &Samplers::SamplersContainer::getMinMaxMag_Linear_UVW_Wrap(uint32_t minMipLevel, uint32_t maxMipLevel) {
	return minMaxMag_Linear_UVW_Wrap[minMipLevel][maxMipLevel];
}

void Samplers::SamplersContainer::initializeMinMaxMagLinearUVWWrap(void) {
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 1;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.mipLodBias = 0.0;

	for (uint32_t i = 0; i < MaxMipLevels; ++i) {
		for (uint32_t j = 0; j < MaxMipLevels; ++j) {
			samplerInfo.minLod = static_cast<float>(i);
			samplerInfo.maxLod = static_cast<float>(j);

			if (vkCreateSampler(device, &samplerInfo, nullptr, &minMaxMag_Linear_UVW_Wrap[i][j]) != VK_SUCCESS) {
				throw std::runtime_error("[DBG] Failed create sampler");
			}
		}
	}
}
