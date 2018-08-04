#include "DrawData.h"
#include "VulkanEngineData.h"
#include "DrawDifferedFramebuffer.h"

#pragma once
namespace DrawDiffered {
	class DrawDifferedBase {

		void initializePipelineDefault(void);
	protected:
		const VulkanEngineApplication::VulkanData * vulkanData;
		const VulkanEngineApplication::VulkanEngineData *vulkanEngineData;
		const std::shared_ptr<Draw::Model> model;

		std::shared_ptr<Draw::DrawDifferedFramebuffer> framebuffer;

		uint32_t drawType;
		VkPipeline drawDifferredPipeline;

		VkPipelineDepthStencilStateCreateInfo depthStancilCreateInfo;
		VkPipelineColorBlendStateCreateInfo mColorBlendStateCreateInfo;
		std::vector<VkPipelineColorBlendAttachmentState> pipelineColorAttachmentState;
		VkPipelineMultisampleStateCreateInfo mMultisampleStateCreateInfo;
		VkPipelineRasterizationStateCreateInfo mRasterizationStateCreateInfo;
		VkGraphicsPipelineCreateInfo mGraphicsPipelineCreateInfoStruct;

		void createGraphicsPipeline(void);
		virtual void initializePipeline(void) = 0;
	public:
		DrawDifferedBase(
			const VulkanEngineApplication::VulkanData * vulkanData,
			const VulkanEngineApplication::VulkanEngineData *vulkanEngineData,
			const std::shared_ptr<Draw::DrawDifferedFramebuffer> &framebuffer,
			const std::shared_ptr<Draw::Model> &model,
			Draw::DrawDataTypes drawType);
		virtual ~DrawDifferedBase() = default;

		virtual void draw(VkCommandBuffer commandBuffer) = 0;
	};
}

