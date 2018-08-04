#include "Texture.h"

#pragma once
namespace Draw {
	class DrawDifferedFramebuffer {
		const VulkanEngineApplication::VulkanData * vulkanData;
		VkRenderPass renderPass;
		VkFramebuffer framebuffer;
		VkExtent3D framebufferSize;

		const static uint32_t textureCount = 4;
		std::array<VkFormat, textureCount> attachmentFormat;
		std::array<std::unique_ptr<Texture>, textureCount> framebufferTextures;

		void createRenderPass(void);
		void createFramebufferTextures(uint32_t width, uint32_t height);
		void createFramebuffer(void);
	public:
		DrawDifferedFramebuffer(
			const VulkanEngineApplication::VulkanData * vulkanData,
			uint32_t width,
			uint32_t height);
		~DrawDifferedFramebuffer();

		void resize(uint32_t width, uint32_t height) = delete;
		//TODO add resize

		uint32_t getFramebufferColorAttachmentCount(void) const;
		VkImage getImageByIndex(uint32_t idx) const;
		VkRenderPass getRenderPass(void) const;
		VkFramebuffer getFramebuffer(void) const;
		VkExtent3D getFramebufferSize(void) const;
	};
}

