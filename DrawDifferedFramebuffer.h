#include "Texture.h"

#pragma once
namespace Draw {
	class DrawDifferedFramebuffer {
		const VulkanEngineApplication::VulkanData * vulkanData;
		VkRenderPass renderPass;
		VkFramebuffer framebuffer;
		VkExtent3D framebufferSize;

		void createRenderPass(void);
		void createFramebufferTextures(uint32_t width, uint32_t height);
		void createFramebuffer(void);
	public:
		DrawDifferedFramebuffer(
			const VulkanEngineApplication::VulkanData * vulkanData,
			uint32_t width,
			uint32_t height);
		~DrawDifferedFramebuffer();

		std::vector<std::unique_ptr<Texture>> framebufferTextures;


		void resize(uint32_t width, uint32_t height) = delete;

		VkRenderPass getRenderPass(void) const;
		VkFramebuffer getFramebuffer(void) const;
		VkExtent3D getFramebufferSize(void) const;
	};
}

