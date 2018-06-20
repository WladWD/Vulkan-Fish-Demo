#include "InitStruct.h"
#include "AssetLoader.h"
#include "DrawImageShaderPipeline.h"
#include "ShaderPipeline.h"
#include "DrawImageUniformStruct.h"
#include "DrawImagePushConstant.h"
#include "VulkanCreater.h"
#include "VulkanEngineData.h"

#include <chrono>

#pragma once
namespace Engine {

	class DrawImage {
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Shader::DrawImageShaderPipeline *shaderPipeline;
		Shader::DrawImageShaderPipeline::PushConstantBuffer pushBuffer;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const std::vector<Shader::DrawImageVertex> mInputVertex = {
			{ { 0.5f,  -0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },
			{ { 0.5f,   0.5f, 0.0f },{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },
			{ { -0.5f,  0.5f, 0.0f },{ 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },
			{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 1.0f, 0.0f },{ 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } }
		};

		const std::vector<uint16_t> inputIndex = {
			0, 3, 1, 2
			//0, 1, 2, 0, 2, 3
		};

		Shader::DrawImageShaderPipeline::UniformBuffer buffer;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		VkPipeline drawImagePipeline;
		VkBuffer vertexBuffer, indexBuffer;
		VkDeviceMemory vertexBufferMemory, indexBufferMemory;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/*VkImageView sourceImageView;
		VkImage sourceImage;
		VkDeviceMemory imageMemory;
		VkExtent3D sourceExtent;*/
		ImageManager::ImageData sourceImage;

		VkImageView nomalSourceImageView;
		VkImage nomalSourceImage;
		VkDeviceMemory nomalImageMemory;
		VkExtent3D nomalSourceExtent;

		VkImageView depthSourceImageView;
		VkImage depthSourceImage;
		VkDeviceMemory depthImageMemory;
		VkExtent3D depthSourceExtent;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const VulkanEngineApplication::VulkanData * vulkanData;
		const VulkanEngineApplication::VulkanEngineData *vulkanEngineData;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void loadImage(const char *imageName, const Asset::AssetLoader *assetLoader, VkImage &sourceImage, VkDeviceMemory &imageMemory, VkImageView &sourceImageView, VkExtent3D &sourceExtent);
		void createImage(const char *data, uint32_t size, VkExtent3D extent, VkFormat format, VkImage &sourceImage, VkDeviceMemory &imageMemory, VkImageView &sourceImageView, VkExtent3D &sourceExtent);
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
		void resize(void);
		void draw(VkCommandBuffer commandBuffer);
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
	};
}

