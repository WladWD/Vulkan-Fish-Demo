#include "VulkanInfo.h"

VulkanEngineApplication::VulkanInfo::VulkanInfo(const VulkanEngineApplication::VulkanData *vulkanData) : vulkanData(const_cast<VulkanEngineApplication::VulkanData *>(vulkanData)) {
}

VulkanEngineApplication::VulkanInfo::~VulkanInfo() {
}

void VulkanEngineApplication::VulkanInfo::printApplicationInfo(void) {
	printInstanceExtensionInfo();
	printDeviceExtensionInfo();
	printInstanceValidationLayerInfo();
	printDeviceValidationLayerInfo();
	printPhisicalDevices();
}

void VulkanEngineApplication::VulkanInfo::printInstanceExtensionInfo(void) {

	std::cout << "[INFO] Instance Extension" << std::endl;
	std::vector<VkExtensionProperties> properties;
	uint32_t propertiesCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr);
	if (propertiesCount > 0) {
		properties.resize(propertiesCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, properties.data());

		for (auto v : properties) {
			std::cout << "[INFO] - " << v.extensionName << ", specVersion = " << v.specVersion << std::endl;
		}
	}
	std::cout << std::endl;
}

void VulkanEngineApplication::VulkanInfo::printDeviceExtensionInfo(void) {
}

void VulkanEngineApplication::VulkanInfo::printPhisicalDevices(void) {

	std::cout << "Enumerate Devices" << std::endl;
	std::vector<VkPhysicalDevice> devices;
	uint32_t deviceCount = 0u;
	vkEnumeratePhysicalDevices(vulkanData->vulkanInstance, &deviceCount, nullptr);
	if (deviceCount > 0) {
		devices.resize(deviceCount);
		vkEnumeratePhysicalDevices(vulkanData->vulkanInstance, &deviceCount, devices.data());
		for (const auto &d : devices) {

			VkPhysicalDeviceFeatures feature;
			vkGetPhysicalDeviceFeatures(d, &feature);

			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(d, &properties);
			std::cout << "[INFO] " << "name: " << properties.deviceName << ", tesselation shader usage " << feature.tessellationShader << std::endl;

		}
	}
	std::cout << std::endl;
}

void VulkanEngineApplication::VulkanInfo::printInstanceValidationLayerInfo(void) {

	std::cout << "[INFO] Instance Validation Layer" << std::endl;
	std::vector<VkLayerProperties> properties;
	uint32_t propertiesCount = 0;
	vkEnumerateInstanceLayerProperties(&propertiesCount, nullptr);
	if (propertiesCount > 0) {
		properties.resize(propertiesCount);
		vkEnumerateInstanceLayerProperties(&propertiesCount, properties.data());

		for (auto v : properties) {
			std::cout << "[INFO] - " << v.layerName << ", specVersion = " << v.specVersion << std::endl;
		}
	}
	std::cout << std::endl;
}

void VulkanEngineApplication::VulkanInfo::printDeviceValidationLayerInfo(void) {
}

