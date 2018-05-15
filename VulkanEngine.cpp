#include "VulkanEngine.h"


Engine::VulkanEngine::VulkanEngine(const VulkanEngineApplication::VulkanData * vulkanData) : vulkanData(vulkanData) {
	drawImage = new DrawImage(vulkanData, &vulkanEngineData);
}

Engine::VulkanEngine::~VulkanEngine() {
	delete vulkanEngineData.quadBuffer;
	delete vulkanEngineData.samplers;

	delete drawImage;
}

void Engine::VulkanEngine::initialize(const Asset::AssetLoader *assetLoader) {
	vulkanEngineData.samplers = new Samplers::SamplersContainer(vulkanData->device);
	vulkanEngineData.quadBuffer = new Engine::QuadBuffer(vulkanData);
	drawImage->initialize(assetLoader);
}

void Engine::VulkanEngine::resize(void) {
}

void Engine::VulkanEngine::pause(void) {
}

void Engine::VulkanEngine::resume(void) {
}

void Engine::VulkanEngine::draw(void) {

	VkCommandBufferBeginInfo mCommandBufferBeginInfo = {};
	mCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	mCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	mCommandBufferBeginInfo.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(vulkanData->commandBuffer[vulkanData->mImageIndex], &mCommandBufferBeginInfo);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Drawing
	drawImage->draw(vulkanData->commandBuffer[vulkanData->mImageIndex], vulkanData->mImageIndex);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (vkEndCommandBuffer(vulkanData->commandBuffer[vulkanData->mImageIndex]) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to record command buffer!");
	}
}
