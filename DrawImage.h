#include "InitStruct.h"
#include "AssetLoader.h"
#include "DrawImageShaderPipeline.h"
#include "ShaderPipeline.h"
#include "DrawImageUniformStruct.h"
#include "DrawImagePushConstant.h"
#include "VulkanCreater.h"
#include "VulkanEngineData.h"
#include "DrawTextLine.h"
#include <chrono>

#pragma once
namespace Engine {

	struct UniformBuffer {
		glm::mat4 projView;
	};

	class DrawImage
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		DrawTextLine *drawText;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Shader::DrawImageShaderPipeline *shaderPipeline;
		Shader::DrawImageShaderPipeline::PushConstantBuffer pushBuffer;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const std::vector<Shader::DrawImageVertex> mInputVertex = {
			{ { 0.5f,  -0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f } },
			{ { 0.5f,   0.5f, 0.0f },{ 0.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } },
			{ { -0.5f,  0.5f, 0.0f },{ 1.0f, 1.0f, 0.0f },{ 0.0f, 1.0f } },
			{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } }
		};

		const std::vector<uint16_t> inputIndex = {
			0, 1, 2, 0, 2, 3
		};

		Shader::DrawImageShaderPipeline::UniformBuffer buffer;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		VkPipeline drawImagePipeline;
		VkBuffer vertexBuffer, indexBuffer;
		VkDeviceMemory vertexBufferMemory, indexBufferMemory;
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
		void initializeVertexBuffer(void);
		void initializeIndexBuffer(void);
		void updateDescriptorSet(void);

		void updateUniforms(void);
		void updatePushConstant(void);
	public:
		DrawImage(const VulkanEngineApplication::VulkanData * vulkanData, const VulkanEngineApplication::VulkanEngineData *vulkanEngineData);
		~DrawImage();
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void initialize(const Asset::AssetLoader *assetLoader);
		void draw(VkCommandBuffer commandBuffer, int32_t frameIdx);
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
	};
}

