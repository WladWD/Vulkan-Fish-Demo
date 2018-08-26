#include "InitStruct.h"
#include <iostream>
#pragma once

namespace VulkanEngineApplication
{
	class VulkanDebug
	{
		/////////////////////////////////////////////////////////////////////////////////////
		VulkanEngineApplication::VulkanData *vulkanData;
		/////////////////////////////////////////////////////////////////////////////////////
		VkDebugReportCallbackEXT mCallbackDebugReport;
		/////////////////////////////////////////////////////////////////////////////////////
		VkResult createDebugReportCallbackEXT(VkInstance mxInstance,
			const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
			const VkAllocationCallbacks *mAllocators,
			VkDebugReportCallbackEXT *pCallback);
		void destroyDebugReportCallbackEXT(VkInstance mxInstance,
			VkDebugReportCallbackEXT pCallback,
			const VkAllocationCallbacks *mAllocators);
		void setupDebugReport();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objType,
			uint64_t obj,
			size_t location,
			int32_t code,
			const char *layerPrefix,
			const char *msg,
			void *userData
		);
		/////////////////////////////////////////////////////////////////////////////////////
	public:
		VulkanDebug(const VulkanEngineApplication::VulkanData *vulkanData);
		~VulkanDebug();
		/////////////////////////////////////////////////////////////////////////////////////
		void initialize(void);
		/////////////////////////////////////////////////////////////////////////////////////
	};
}

