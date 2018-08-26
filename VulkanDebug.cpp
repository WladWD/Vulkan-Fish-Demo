#include "VulkanDebug.h"


VulkanEngineApplication::VulkanDebug::VulkanDebug(const VulkanEngineApplication::VulkanData * vulkanData) : vulkanData(const_cast<VulkanEngineApplication::VulkanData *>(vulkanData)) {
}

VulkanEngineApplication::VulkanDebug::~VulkanDebug() {
	destroyDebugReportCallbackEXT(vulkanData->vulkanInstance, mCallbackDebugReport, nullptr);
}

void VulkanEngineApplication::VulkanDebug::initialize(void) {
	setupDebugReport();
}

VkResult VulkanEngineApplication::VulkanDebug::createDebugReportCallbackEXT(VkInstance mxInstance,
	const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
	const VkAllocationCallbacks *mAllocators,
	VkDebugReportCallbackEXT *pCallback)
{
	auto mFunc = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
		vkGetInstanceProcAddr(mxInstance, "vkCreateDebugReportCallbackEXT"));
	if (mFunc != nullptr)
		return mFunc(mxInstance, pCreateInfo, mAllocators, pCallback);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanEngineApplication::VulkanDebug::destroyDebugReportCallbackEXT(
	VkInstance mxInstance,
	VkDebugReportCallbackEXT pCallback,
	const VkAllocationCallbacks * mAllocators)
{
	auto mFunc = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT> (
		vkGetInstanceProcAddr(mxInstance, "vkDestroyDebugReportCallbackEXT"));
	if (mFunc != nullptr)
		mFunc(mxInstance, pCallback, mAllocators);
	else
		throw std::runtime_error("[ERR]\t vkDestroyDebugReportCallbackEXT is not find!");
}

void VulkanEngineApplication::VulkanDebug::setupDebugReport(void)
{
	VkDebugReportCallbackCreateInfoEXT mDebugReportCallbackCreateInfoStruct = {};
	mDebugReportCallbackCreateInfoStruct.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	mDebugReportCallbackCreateInfoStruct.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;
	mDebugReportCallbackCreateInfoStruct.pfnCallback = debugCallback;
	mDebugReportCallbackCreateInfoStruct.pUserData = this;

	if (createDebugReportCallbackEXT(vulkanData->vulkanInstance, &mDebugReportCallbackCreateInfoStruct, nullptr, &mCallbackDebugReport) != VK_SUCCESS) {
		throw std::runtime_error("Failed to set up debug callback!");
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanEngineApplication::VulkanDebug::debugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char * layerPrefix,
	const char * msg,
	void * userData)
{
	std::cout << "[DBG]\tValidation Layer: " << msg << std::endl;

	return VK_FALSE;
}
