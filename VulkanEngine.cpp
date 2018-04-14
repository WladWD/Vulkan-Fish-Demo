#include "VulkanEngine.h"


Engine::VulkanEngine::VulkanEngine(const VulkanEngineApplication::VulkanData * vulkanData, JNIEnv * env, jobject rmng) : vulkanData(vulkanData) {
	drawImage = new DrawImage(vulkanData);
}

Engine::VulkanEngine::~VulkanEngine() {
	delete drawImage;
}

void Engine::VulkanEngine::initialize(void) {
	createCommandPool();
	createCommandBuffer();
	createSemaphore();

	initializeDraw();
}

void Engine::VulkanEngine::createCommandPool(void) {
	VkCommandPoolCreateInfo mCommandPoolCreateInfoStruct = {};
	mCommandPoolCreateInfoStruct.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	mCommandPoolCreateInfoStruct.queueFamilyIndex = vulkanData->queueFamilyIndex;

	if (vkCreateCommandPool(vulkanData->device, &mCommandPoolCreateInfoStruct, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to create command pool!");
	}
}

void Engine::VulkanEngine::createCommandBuffer(void) {
	commandBuffer.resize(vulkanData->swapchainImage.size());

	VkCommandBufferAllocateInfo mCommandBufferAllocateInfoStruct = {};
	mCommandBufferAllocateInfoStruct.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	mCommandBufferAllocateInfoStruct.commandPool = commandPool;
	mCommandBufferAllocateInfoStruct.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	mCommandBufferAllocateInfoStruct.commandBufferCount = static_cast<uint32_t>(commandBuffer.size());

	if (vkAllocateCommandBuffers(vulkanData->device, &mCommandBufferAllocateInfoStruct, commandBuffer.data()) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to allocate command buffer!");
	}

	for (size_t i = 0; i < commandBuffer.size(); ++i)
	{
		VkCommandBufferBeginInfo mCommandBufferBeginInfo = {};
		mCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		mCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		mCommandBufferBeginInfo.pInheritanceInfo = nullptr;

		vkBeginCommandBuffer(commandBuffer[i], &mCommandBufferBeginInfo);

		drawImage->draw(commandBuffer[i], i);

		if (vkEndCommandBuffer(commandBuffer[i]) != VK_SUCCESS) {
			throw std::runtime_error("[DBG]\tFailed to record command buffer!");
		}
	}
}

void Engine::VulkanEngine::createSemaphore(void) {
	VkSemaphoreCreateInfo mSemaphoreCreateInfo = {};
	mSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(vulkanData->device, &mSemaphoreCreateInfo, nullptr, &imageAvailable) != VK_SUCCESS ||
		vkCreateSemaphore(vulkanData->device, &mSemaphoreCreateInfo, nullptr, &renderFinish) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to create semaphores!");
	}
}

void Engine::VulkanEngine::initializeDraw(void) {
	drawImage->initialize();
}

void Engine::VulkanEngine::resize(void) {
}

void Engine::VulkanEngine::pause(void) {
}

void Engine::VulkanEngine::resume(void) {
}

void Engine::VulkanEngine::draw(void) {

	vkQueueWaitIdle(vulkanData->queue[0]);

	uint32_t mImageIndex = 0;
	VkResult mResult = vkAcquireNextImageKHR(vulkanData->device, vulkanData->swapchain, std::numeric_limits<uint64_t>::max(), imageAvailable, VK_NULL_HANDLE, &mImageIndex);

	if (mResult == VK_ERROR_OUT_OF_DATE_KHR) {
		resize();
		return;
	}
	else if (mResult != VK_SUCCESS && mResult != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("[DBG]\tFailed to aquire swap chain images!");
	}

	VkSubmitInfo mSubmitInfo = {};
	mSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore mWaitSemaphore[] = { imageAvailable };
	VkPipelineStageFlags mWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	mSubmitInfo.waitSemaphoreCount = 1;
	mSubmitInfo.pWaitSemaphores = mWaitSemaphore;
	mSubmitInfo.pWaitDstStageMask = mWaitStages;
	mSubmitInfo.commandBufferCount = 1;
	mSubmitInfo.pCommandBuffers = &commandBuffer[mImageIndex];

	VkSemaphore mSigmalSemaphores[] = { renderFinish };
	mSubmitInfo.pSignalSemaphores = mSigmalSemaphores;
	mSubmitInfo.signalSemaphoreCount = 1;

	VkSwapchainKHR mSwapChains[] = { vulkanData->swapchain };

	VkPresentInfoKHR mPresentInfo = {};
	mPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	mPresentInfo.waitSemaphoreCount = 1;
	mPresentInfo.pWaitSemaphores = mSigmalSemaphores;
	mPresentInfo.swapchainCount = 1;
	mPresentInfo.pSwapchains = mSwapChains;
	mPresentInfo.pImageIndices = &mImageIndex;
	mPresentInfo.pResults = nullptr;

	VkResult error = vkQueueSubmit(vulkanData->queue[0], 1, &mSubmitInfo, VK_NULL_HANDLE);
	if (error != VK_SUCCESS) {
		throw std::runtime_error("[DGB]\tFailed to submit draw command to buffer!");
	}

	vkQueuePresentKHR(vulkanData->queue[0], &mPresentInfo);
}
