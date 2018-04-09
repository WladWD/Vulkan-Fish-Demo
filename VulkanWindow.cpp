#include "VulkanWindow.h"

VulkanEngineApplication::VulkanWindow::VulkanWindow(int32_t width, int32_t height, const char * sTitle) {
	initWindow(width, height, sTitle);
	initVulkan();

#ifdef _DEBUG
	printApplicationInfo();
#endif

}

VulkanEngineApplication::VulkanWindow::~VulkanWindow() {
	
	vkDestroySurfaceKHR(vulkanInstance, windowSurface, nullptr);

	destroyDebugReportCallbackEXT(vulkanInstance, mCallbackDebugReport, nullptr);

	vkDestroyInstance(vulkanInstance, nullptr);

	glfwDestroyWindow(mWindow);

	glfwTerminate();
}

void VulkanEngineApplication::VulkanWindow::initWindow(int32_t width, int32_t height, const char * sTitle) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	mWindow = glfwCreateWindow(width, height, sTitle, nullptr, nullptr);

	glfwSetWindowUserPointer(mWindow, this);
	glfwSetWindowSizeCallback(mWindow, OnWindowResize);
}

void VulkanEngineApplication::VulkanWindow::initVulkan(void) {
	createInstance();
	setupDebugReport();
	createWindowSurface();
	createDevice();
	createSwapShain();
}

void VulkanEngineApplication::VulkanWindow::createInstance(void) {

	VkApplicationInfo appInfo = { };
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.pApplicationName = "FishDemo";
	appInfo.pEngineName = "RGBEngine";
	appInfo.pNext = nullptr;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	VkInstanceCreateInfo instanceInfo = { };
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	
	if (checkValidationSupport() && enableInstanceValidationLayer) {
		instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceValidationLayer.size());
		instanceInfo.ppEnabledLayerNames = instanceValidationLayer.data();
	}
	else {
		instanceInfo.enabledLayerCount = 0;
		instanceInfo.ppEnabledLayerNames = nullptr;
	}

	addInstanceRequiredExtension();
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtension.size());
	instanceInfo.ppEnabledExtensionNames = instanceExtension.data();

	if (vkCreateInstance(&instanceInfo, nullptr, &vulkanInstance) != VK_SUCCESS) {
		throw std::runtime_error("[Error] Failed create instace!");
	}
}

void VulkanEngineApplication::VulkanWindow::addInstanceRequiredExtension(void) {

	uint32_t glfwExtensionCount = 0;
	const char **glfwExtension;

	glfwExtension = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		instanceExtension.push_back(glfwExtension[i]);

	if (enableInstanceValidationLayer)
		instanceExtension.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
}

void VulkanEngineApplication::VulkanWindow::selectPhisicalDevice(void) {

}

bool VulkanEngineApplication::VulkanWindow::checkValidationSupport(void) {

	std::vector<VkLayerProperties> properties;
	uint32_t propertiesCount = 0;
	vkEnumerateInstanceLayerProperties(&propertiesCount, nullptr);
	if (propertiesCount > 0) {
		properties.resize(propertiesCount);
		vkEnumerateInstanceLayerProperties(&propertiesCount, properties.data());

		int32_t findCount = 0;
		for (auto ls : instanceValidationLayer) {
			for (auto al : properties) {
				findCount += !strcmp(ls, al.layerName);
			}
		}
		return findCount == instanceValidationLayer.size();
	}

	return false;
}

void VulkanEngineApplication::VulkanWindow::createWindowSurface(void) {
	if (glfwCreateWindowSurface(vulkanInstance, mWindow, nullptr, &windowSurface) != VK_SUCCESS) {
		throw std::runtime_error("[Error] Failed to create window surface!");
	}
}

void VulkanEngineApplication::VulkanWindow::createDevice(void) {
	selectPhisicalDevice();
}

void VulkanEngineApplication::VulkanWindow::createSwapShain(void) {
}

VkResult VulkanEngineApplication::VulkanWindow::createDebugReportCallbackEXT(VkInstance mxInstance,
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

void VulkanEngineApplication::VulkanWindow::destroyDebugReportCallbackEXT(
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

void VulkanEngineApplication::VulkanWindow::setupDebugReport(void)
{
	if (!enableInstanceValidationLayer)
		return;

	VkDebugReportCallbackCreateInfoEXT mDebugReportCallbackCreateInfoStruct = {};
	mDebugReportCallbackCreateInfoStruct.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	mDebugReportCallbackCreateInfoStruct.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;
	mDebugReportCallbackCreateInfoStruct.pfnCallback = debugCallback;
	mDebugReportCallbackCreateInfoStruct.pUserData = this;

	if (createDebugReportCallbackEXT(vulkanInstance, &mDebugReportCallbackCreateInfoStruct, nullptr, &mCallbackDebugReport) != VK_SUCCESS)
		throw std::runtime_error("Failed to set up debug callback!");
}


void VulkanEngineApplication::VulkanWindow::OnWindowResize(GLFWwindow * mWindow, int width, int height)
{
	if (!width || !height)
		return;

	VulkanEngineApplication::VulkanWindow *mApplication = static_cast<VulkanEngineApplication::VulkanWindow *>(glfwGetWindowUserPointer(mWindow));
	//mApplication->RecreateSwapChain();
}

void VulkanEngineApplication::VulkanWindow::printApplicationInfo(void) {
	printInstanceExtensionInfo();
	printDeviceExtensionInfo();
	printInstanceValidationLayerInfo();
	printDeviceValidationLayerInfo();
}

void VulkanEngineApplication::VulkanWindow::printInstanceExtensionInfo(void) {

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

void VulkanEngineApplication::VulkanWindow::printDeviceExtensionInfo(void) {
}

void VulkanEngineApplication::VulkanWindow::printInstanceValidationLayerInfo(void) {

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

void VulkanEngineApplication::VulkanWindow::printDeviceValidationLayerInfo(void)
{
}


void VulkanEngineApplication::VulkanWindow::run() {
}


VKAPI_ATTR VkBool32 VKAPI_CALL VulkanEngineApplication::VulkanWindow::debugCallback(
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
