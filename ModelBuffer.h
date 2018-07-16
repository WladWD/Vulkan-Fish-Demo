#include "InitStruct.h"

#pragma once
namespace Draw {
	class ModelBuffer {
	private:
		const VulkanEngineApplication::VulkanData * vulkanData;

		VkDeviceMemory vertexBufferMemory;
		VkDeviceMemory indexBufferMemory;

		VkBuffer vertexBuffer;
		VkBuffer indexBuffer;
	public:
	
		ModelBuffer(const VulkanEngineApplication::VulkanData * vulkanData,
			VkBuffer vertexBuffer, VkBuffer indexBuffer,
			VkDeviceMemory vertexBufferMemory, VkDeviceMemory indexBufferMemory);
		~ModelBuffer();

		VkBuffer getVertexBuffer(void) const;
		VkBuffer getIndexBuffer(void) const;
	};
}

