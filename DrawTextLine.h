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
#include "SpecConstantDrawText.h"
#include <chrono>

#pragma once
namespace Engine {
	class DrawTextLine {
	public:
		enum class DrawTextAligment {
			AligmentWidth,
			AligmnetHeight
		};

		enum class DrawTextBindingPoint {
			LeftTop,
			LeftBotton,
			RightTop,
			RightBotton
		};
	private:
		Shader::DrawTextLineShaderPipeline *shaderPipeline;
		Shader::DrawTextLineShaderPipeline::PushConstantBuffer pushBuffer;
		
		VkPipeline drawImagePipeline;
		VkImageView sourceImageView;
		VkImage sourceImage;
		VkDeviceMemory imageMemory;
		VkExtent3D sourceExtent;
		
		const VulkanEngineApplication::VulkanData * vulkanData;
		const VulkanEngineApplication::VulkanEngineData *vulkanEngineData;

	private:
		void loadImage(const char *imageName, const Asset::AssetLoader *assetLoader);
		void createImage(const char *data, uint32_t size, VkExtent3D extent, VkFormat format);
		void initializePipeline(void);
		void updateDescriptorSet(void);
	public:
		DrawTextLine(const VulkanEngineApplication::VulkanData * vulkanData, const VulkanEngineApplication::VulkanEngineData *vulkanEngineData);
		~DrawTextLine();
		
		void resize(void);
		void initialize(const Asset::AssetLoader *assetLoader);
		void setMessage(std::string text, glm::vec2 position, float scale, DrawTextAligment aligment = DrawTextAligment::AligmentWidth, DrawTextBindingPoint bindingPoint = DrawTextBindingPoint::LeftTop);
		void draw(VkCommandBuffer commandBuffer);
	};
}

