#include "InitStruct.h"
#include "AssetLoader.h"
#include "DrawImageShaderPipeline.h"
#include "ShaderPipeline.h"
#include "DrawImageUniformStruct.h"
#include <chrono>

#pragma once
namespace Engine 
{
	class DrawImage
	{
		Shader::DrawImageShaderPipeline *shaderPipeline;
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
		VkBuffer vertexBuffer, indexBuffer, uniformBuffer;
		VkDeviceMemory vertexBufferMemory, indexBufferMemory, uniformBufferMemory;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		VkSampler sampler;
		VkImageView sourceImageView;
		VkImage sourceImage;
		VkDeviceMemory imageMemory;
		VkExtent3D sourceExtent;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const VulkanEngineApplication::VulkanData * vulkanData;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void loadImage(VkCommandBuffer commandBuffer, const char *imageName, const Asset::AssetLoader *assetLoader);
		void createImage(VkCommandBuffer commandBuffer, const char *data, uint32_t size, VkExtent3D extent, VkFormat format);
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory& bufferMemory);

		void initializePipeline(void);
		void initializeVertexBuffer(VkCommandBuffer commandBuffer);
		void initializeIndexBuffer(VkCommandBuffer commandBuffer);
		void initializeUniformBuffer(void);
		void initializeSampler(void);
		void updateDescriptorSet(void);

		void updateUniforms(void);
	public:
		DrawImage(const VulkanEngineApplication::VulkanData * vulkanData);
		~DrawImage();
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void initialize(VkCommandBuffer initializeCommandBuffer, const Asset::AssetLoader *assetLoader);
		void draw(VkCommandBuffer commandBuffer, int32_t frameIdx);
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
	};
}

