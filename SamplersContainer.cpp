#include "SamplersContainer.h"

Samplers::SamplersContainer::SamplersContainer(VkDevice device): device(device), minMaxMag_Linear_UVW_Wrap(VK_NULL_HANDLE){
	initializeMinMaxMagLinearUVWWrap();
}

Samplers::SamplersContainer::~SamplersContainer() {
	vkDestroySampler(device, minMaxMag_Linear_UVW_Wrap, nullptr);
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
	samplerInfo.mipLodBias = 0.0;
	samplerInfo.minLod = 0.0;
	samplerInfo.maxLod = 0.0;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	if (vkCreateSampler(device, &samplerInfo, nullptr, &minMaxMag_Linear_UVW_Wrap) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed create sampler");
	}
}
