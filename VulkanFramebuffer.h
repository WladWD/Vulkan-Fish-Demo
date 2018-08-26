#include "InitStruct.h"
#pragma once

namespace VulkanEngineApplication
{
	class VulkanFramebuffer {
		VulkanEngineApplication::VulkanData *vulkanData;
		
		
		void createRenderPass(void);
		void createFramebuffer(void);
	public:
		VulkanFramebuffer(const VulkanEngineApplication::VulkanData *vulkanData);
		~VulkanFramebuffer();
		
		void initialize(void);
		void free(void);
	};
}

