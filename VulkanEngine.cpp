#include "VulkanEngine.h"


Engine::VulkanEngine::VulkanEngine(const VulkanEngineApplication::VulkanData * vulkanData) : vulkanData(vulkanData) {
	drawImage = new DrawImage(vulkanData);
}

Engine::VulkanEngine::~VulkanEngine() {
	delete drawImage;
}

VkCommandBuffer Engine::VulkanEngine::beginSingleTimeCommand(void)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandBufferCount = 1;
	allocInfo.commandPool = vulkanData->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VkCommandBuffer copyCommandBuffer;
	vkAllocateCommandBuffers(vulkanData->device, &allocInfo, &copyCommandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(copyCommandBuffer, &beginInfo);

	return copyCommandBuffer;
}

void Engine::VulkanEngine::endSingleTimeCommand(VkCommandBuffer initCommandBuffer)
{
	vkEndCommandBuffer(initCommandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &initCommandBuffer;

	vkQueueSubmit(vulkanData->graphicsQueue[0], 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(vulkanData->graphicsQueue[0]);

	vkFreeCommandBuffers(vulkanData->device, vulkanData->commandPool, 1, &initCommandBuffer);
}

void Engine::VulkanEngine::initialize(const Asset::AssetLoader *assetLoader) {
	VkCommandBuffer initializeCommandBuffer = beginSingleTimeCommand();
	drawImage->initialize(initializeCommandBuffer, assetLoader);
	endSingleTimeCommand(initializeCommandBuffer);
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
