#include "VulkanHeader.h"

#pragma once
namespace VulkanEngineApplication
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR mCapabilities;
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
	};

	struct UniformBuffer {
		glm::mat4 projView;
	};
}
