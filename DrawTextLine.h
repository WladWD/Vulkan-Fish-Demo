#include "InitStruct.h"
#include "AssetLoader.h"
#include "DrawImageShaderPipeline.h"
#include "ShaderPipeline.h"
#include "DrawImageUniformStruct.h"
#include "DrawImagePushConstant.h"
#include "VulkanCreater.h"
#include "VulkanEngineData.h"
#include "DrawTextLineShaderPipeline.h"
#include "DrawTextPushConstant.h"
#include <chrono>

#pragma once
namespace Engine {
	class DrawTextLine {
		Shader::DrawTextLineShaderPipeline *shaderPipeline;
		Shader::DrawTextLineShaderPipeline::PushConstantBuffer pushBuffer;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		VkPipeline drawImagePipeline;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		VkImageView sourceImageView;
		VkImage sourceImage;
		VkDeviceMemory imageMemory;
		VkExtent3D sourceExtent;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const VulkanEngineApplication::VulkanData * vulkanData;
		const VulkanEngineApplication::VulkanEngineData *vulkanEngineData;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void loadImage(const char *imageName, const Asset::AssetLoader *assetLoader);
		void createImage(const char *data, uint32_t size, VkExtent3D extent, VkFormat format);
		void initializePipeline(void);
		void updateDescriptorSet(void);
		void updatePushConstant(std::string text, glm::vec2 offest, glm::vec2 scale);
	public:
		DrawTextLine(const VulkanEngineApplication::VulkanData * vulkanData, const VulkanEngineApplication::VulkanEngineData *vulkanEngineData);
		~DrawTextLine();
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void initialize(const Asset::AssetLoader *assetLoader);
		void draw(VkCommandBuffer commandBuffer, int32_t frameIdx, std::string text, glm::vec2 offest, glm::vec2 scale);
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
	};
}

