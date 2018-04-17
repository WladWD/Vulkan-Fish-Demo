#include "VulkanCommand.h"


VulkanEngineApplication::VulkanCommand::VulkanCommand(const VulkanEngineApplication::VulkanData * vulkanData) : vulkanData(const_cast<VulkanEngineApplication::VulkanData *>(vulkanData)) {
}

VulkanEngineApplication::VulkanCommand::~VulkanCommand() {
	free();
}

void VulkanEngineApplication::VulkanCommand::createCommandPool(void) {
	VkCommandPoolCreateInfo mCommandPoolCreateInfoStruct = {};
	mCommandPoolCreateInfoStruct.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	mCommandPoolCreateInfoStruct.queueFamilyIndex = vulkanData->queueFamilyIndex;
	mCommandPoolCreateInfoStruct.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(vulkanData->device, &mCommandPoolCreateInfoStruct, nullptr, &vulkanData->commandPool) != VK_SUCCESS) {
		throw std::runtime_error("[DBG]\tFailed to create command pool!");
	}
}

void VulkanEngineApplication::VulkanCommand::createCommandBuffer(void) {
	vulkanData->commandBuffer.resize(vulkanData->swapchainImage.size());

	VkCommandBufferAllocateInfo mCommandBufferAllocateInfoStruct = {};
	mCommandBufferAllocateInfoStruct.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	mCommandBufferAllocateInfoStruct.commandPool = vulkanData->commandPool;
	mCommandBufferAllocateInfoStruct.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	mCommandBufferAllocateInfoStruct.commandBufferCount = static_cast<uint32_t>(vulkanData->commandBuffer.size());

	if (vkAllocateCommandBuffers(vulkanData->device, &mCommandBufferAllocateInfoStruct, vulkanData->commandBuffer.data()) != VK_SUCCESS) {
		throw std::runtime_error("[DBG] Failed to allocate command buffer!");
	}
}

void VulkanEngineApplication::VulkanCommand::initialize(void) {
	createCommandPool();
	createCommandBuffer();
}

void VulkanEngineApplication::VulkanCommand::free(void) {
	vkDestroyCommandPool(vulkanData->device, vulkanData->commandPool, nullptr);
}
