#include "InitStruct.h"

#pragma once
namespace Draw {
	class ModelBuffer {
	private:
		const VulkanEngineApplication::VulkanData * vulkanData;

		const VkDeviceMemory vertexBufferMemory;
		const VkDeviceMemory indexBufferMemory;

	public:
		const VkBuffer vertexBuffer;
		const VkBuffer indexBuffer;

		const VkDeviceSize vertexBufferSize;
		const VkDeviceSize indexBufferSize;
	public:
		ModelBuffer(const VulkanEngineApplication::VulkanData * vulkanData,
			VkBuffer vertexBuffer, VkBuffer indexBuffer,
			VkDeviceMemory vertexBufferMemory, VkDeviceMemory indexBufferMemory,
			VkDeviceSize vertexBufferSize, VkDeviceSize indexBufferSize);
		~ModelBuffer();

		//VkBuffer getVertexBuffer(void) const;
		//VkBuffer getIndexBuffer(void) const;
	};
}

