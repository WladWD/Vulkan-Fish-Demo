#include "VulkanHeader.h"

#pragma once
namespace Samplers {
	class SamplersContainer {
		static const uint32_t MaxMipLevels = 14;
		VkDevice device;

		VkSampler minMaxMag_Linear_UVW_Wrap[MaxMipLevels][MaxMipLevels];

		void initializeMinMaxMagLinearUVWWrap(void);
	public:
		SamplersContainer(VkDevice device);
		~SamplersContainer();

		const VkSampler &getMinMaxMag_Linear_UVW_Wrap(uint32_t minMipLevel, uint32_t maxMipLevel);
	};
}

