#include "VulkanEngine.h"

Engine::VulkanEngine::VulkanEngine(const VulkanEngineApplication::VulkanData * vulkanData):
	vulkanData(vulkanData), scene(nullptr) {
	drawImage = new DrawImage(vulkanData, &vulkanEngineData);
	drawFPS = new DrawFPS(vulkanData, &vulkanEngineData);

}

Engine::VulkanEngine::~VulkanEngine() {
	delete vulkanEngineData.quadBuffer;
	delete vulkanEngineData.samplers;
	delete vulkanEngineData.imageContainer;

	delete drawImage;
	delete drawFPS;
}

void Engine::VulkanEngine::initialize(const Asset::AssetLoader *assetLoader) {
	initializeEngine(assetLoader);

	std::unique_ptr<LoadManager::LoadModel> loader =
		std::unique_ptr<LoadManager::LoadModel>(
			new LoadManager::LoadModel(
				assetLoader,
				vulkanData));
	scene = loader->getLoadedScene();

	drawImage->initialize(assetLoader);
	drawFPS->initialize(assetLoader);
}

void Engine::VulkanEngine::resize(void) {
	drawImage->resize();
	drawFPS->resize();
}

void Engine::VulkanEngine::pause(void) {
}

void Engine::VulkanEngine::resume(void) {
}

void Engine::VulkanEngine::initializeEngine(const Asset::AssetLoader *assetLoader) {
	vulkanEngineData.samplers = new Samplers::SamplersContainer(vulkanData->device);
	vulkanEngineData.quadBuffer = new Engine::QuadBuffer(vulkanData);
	vulkanEngineData.imageContainer = new ImageManager::ImageContainer(assetLoader, vulkanData);
}

void Engine::VulkanEngine::drawToSwapChainTexture(void) {
	std::array<VkClearValue, 2> mClearValue;
	mClearValue[0].color = { 1.0f, 1.0f, 0.0f, 1.0f };
	mClearValue[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo mRenderPassBeginInfo = {};
	mRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	mRenderPassBeginInfo.renderPass = vulkanData->swapchainRenderpass;
	mRenderPassBeginInfo.framebuffer = vulkanData->swapchainFramebuffer[vulkanData->mImageIndex];
	mRenderPassBeginInfo.renderArea.offset = { 0, 0 };
	mRenderPassBeginInfo.renderArea.extent.width = vulkanData->mSwapChainImageExtent.width;
	mRenderPassBeginInfo.renderArea.extent.height = vulkanData->mSwapChainImageExtent.height;
	mRenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(mClearValue.size());
	mRenderPassBeginInfo.pClearValues = mClearValue.data();

	vkCmdBeginRenderPass(vulkanData->commandBuffer[vulkanData->mImageIndex], &mRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	//Degin Drawing
	drawImage->draw(vulkanData->commandBuffer[vulkanData->mImageIndex]);
	drawFPS->draw(vulkanData->commandBuffer[vulkanData->mImageIndex]);
	//End Drawwing

	vkCmdEndRenderPass(vulkanData->commandBuffer[vulkanData->mImageIndex]);
}

void Engine::VulkanEngine::draw(void) {

	VkCommandBufferBeginInfo mCommandBufferBeginInfo = {};
	mCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	mCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	mCommandBufferBeginInfo.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(vulkanData->commandBuffer[vulkanData->mImageIndex], &mCommandBufferBeginInfo);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Draw Deffered

	//Draw to Swapchain
	drawToSwapChainTexture();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (vkEndCommandBuffer(vulkanData->commandBuffer[vulkanData->mImageIndex]) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to record command buffer!");
	}
}
