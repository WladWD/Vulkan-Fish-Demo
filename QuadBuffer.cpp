#include "QuadBuffer.h"


Engine::QuadBuffer::QuadBuffer(const VulkanEngineApplication::VulkanData * vulkanData): vulkanData(vulkanData) {
	createVertexBuffer();
	createIndexBuffer();
}

Engine::QuadBuffer::~QuadBuffer() {
	vkDestroyBuffer(vulkanData->device, vertexBuffer, nullptr);
	vkDestroyBuffer(vulkanData->device, indexBuffer, nullptr);

	vkFreeMemory(vulkanData->device, vertexBufferMemory, nullptr);
	vkFreeMemory(vulkanData->device, indexBufferMemory, nullptr);
}

void Engine::QuadBuffer::createVertexBuffer(void) {
	VkDeviceSize bufferSize = sizeof(VertexFormat::VertexPosition) * mInputVertex.size();

	VulkanInitialize::createInitializeDeviceLocalBuffer(vulkanData,
		vulkanData->commandPool,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferSize,
		mInputVertex.data(),
		vertexBuffer,
		vertexBufferMemory);
}

void Engine::QuadBuffer::createIndexBuffer(void) {
	VkDeviceSize bufferSize = sizeof(uint16_t) * inputIndex.size();

	VulkanInitialize::createInitializeDeviceLocalBuffer(vulkanData,
		vulkanData->commandPool,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferSize,
		inputIndex.data(),
		indexBuffer,
		indexBufferMemory);
}

VkBuffer Engine::QuadBuffer::getVertexBuffer(void) {
	return vertexBuffer;
}

VkBuffer Engine::QuadBuffer::getIndexBuffer(void) {
	return indexBuffer;
}

VkDeviceMemory Engine::QuadBuffer::getVertexBufferMemory(void) {
	return vertexBufferMemory;
}

VkDeviceMemory Engine::QuadBuffer::getIndexBufferMemory(void) {
	return indexBufferMemory;
}
