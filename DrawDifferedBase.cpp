#include "DrawDifferedBase.h"

DrawDiffered::DrawDifferedBase::DrawDifferedBase(
	const VulkanEngineApplication::VulkanData * vulkanData,
	const VulkanEngineApplication::VulkanEngineData * vulkanEngineData,
	const std::shared_ptr<Draw::Model>& model,
	Draw::DrawDataTypes drawType):
	vulkanData(vulkanData), vulkanEngineData(vulkanEngineData),
	model(model), drawType(static_cast<uint32_t>(drawType)) {
}
