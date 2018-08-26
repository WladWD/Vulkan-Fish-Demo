#include "DrawFPS.h"

Engine::DrawFPS::DrawFPS(const VulkanEngineApplication::VulkanData * vulkanData, const VulkanEngineApplication::VulkanEngineData * vulkanEngineData):
	vulkanData(vulkanData), vulkanEngineData(vulkanEngineData), drawText(nullptr) {
	drawText = new DrawTextLine(vulkanData, vulkanEngineData);
}

Engine::DrawFPS::~DrawFPS() {
	delete drawText;
}

void Engine::DrawFPS::resize(void) {
	drawText->resize();
}

void Engine::DrawFPS::initialize(const Asset::AssetLoader * assetLoader) {
	drawText->initialize(assetLoader);
}

void Engine::DrawFPS::draw(VkCommandBuffer commandBuffer) {
	static uint64_t frameCount = 0;
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto curTime = std::chrono::high_resolution_clock::now();
	double dt = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count()); ++frameCount;

	static double fps = 0.0;
	if (dt >= 1000.0) {
		fps = static_cast<double>(frameCount) / dt * 1000.0;
		startTime = curTime;
		frameCount = 0;
	}

	static char buf[10];
	sprintf(buf, "%.4lf", fps);
	//LeftBotton LeftTop AligmentWidth AligmnetHeight
	float scale = vulkanData->mSwapChainImageExtent.width > vulkanData->mSwapChainImageExtent.height ? 0.25f : 0.5f;
	drawText->setMessage(buf, glm::vec2(1.0, 1.0), scale, DrawTextLine::DrawTextAligment::AligmentWidth, DrawTextLine::DrawTextBindingPoint::RightBotton);

	drawText->draw(commandBuffer);
}
