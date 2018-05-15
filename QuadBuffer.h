#include "VertexBufferFormat.h"
#include "VulkanCreater.h"

#pragma once
namespace Engine {
	class QuadBuffer {
		const VulkanEngineApplication::VulkanData * vulkanData;

		const std::vector<VertexFormat::VertexPosition> mInputVertex = {
			{ {  1.0f,  -1.0f, 0.0f }},
			{ {  1.0f,   1.0f, 0.0f }},
			{ { -1.0f,   1.0f, 0.0f }},
			{ { -1.0f,  -1.0f, 0.0f }}
		};

		const std::vector<uint16_t> inputIndex = {
			0, 1, 2, 0, 2, 3
		};

		VkBuffer vertexBuffer, indexBuffer;
		VkDeviceMemory vertexBufferMemory, indexBufferMemory;

		void createVertexBuffer(void);
		void createIndexBuffer(void);
	public:
		QuadBuffer(const VulkanEngineApplication::VulkanData * vulkanData);
		~QuadBuffer();

		VkBuffer getVertexBuffer(void);
		VkBuffer getIndexBuffer(void);

		VkDeviceMemory getVertexBufferMemory(void);
		VkDeviceMemory getIndexBufferMemory(void);
	};
}

