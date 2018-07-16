#include "ModelBuffer.h"

Draw::ModelBuffer::ModelBuffer(
	const VulkanEngineApplication::VulkanData * vulkanData,
	VkBuffer vertexBuffer, VkBuffer indexBuffer,
	VkDeviceMemory vertexBufferMemory, VkDeviceMemory indexBufferMemory):
	vulkanData(vulkanData),	vertexBuffer(vertexBuffer), indexBuffer(indexBuffer) { }

Draw::ModelBuffer::~ModelBuffer() {
	vkDestroyBuffer(vulkanData->device, vertexBuffer, nullptr);
	vkDestroyBuffer(vulkanData->device, indexBuffer, nullptr);

	vkFreeMemory(vulkanData->device, vertexBufferMemory, nullptr);
	vkFreeMemory(vulkanData->device, indexBufferMemory, nullptr);
}

VkBuffer Draw::ModelBuffer::getVertexBuffer(void) const {
	return vertexBuffer;
}

VkBuffer Draw::ModelBuffer::getIndexBuffer(void) const {
	return indexBuffer;
}
