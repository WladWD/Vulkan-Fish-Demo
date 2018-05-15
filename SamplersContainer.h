#include "VulkanHeader.h"

#pragma once
namespace Samplers {
	class SamplersContainer {
		VkDevice device;

		void initializeMinMaxMagLinearUVWWrap(void);
	public:
		SamplersContainer(VkDevice device);
		~SamplersContainer();

		VkSampler minMaxMag_Linear_UVW_Wrap;
	};
}

