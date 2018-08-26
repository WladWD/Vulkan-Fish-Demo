#include "QuadBuffer.h"
#include "ImageContainer.h"

#pragma once
namespace VulkanEngineApplication {
	struct VulkanEngineData {
		ImageManager::ImageContainer *imageContainer;
		Samplers::SamplersContainer *samplers;
		Engine::QuadBuffer *quadBuffer;

		VulkanEngineData():	imageContainer(nullptr), samplers(nullptr), quadBuffer(nullptr) {}
	};
}
