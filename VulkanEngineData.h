#include "QuadBuffer.h"

#pragma once
namespace VulkanEngineApplication {
	struct VulkanEngineData {
		//////////////////////////////////////////////////
		Samplers::SamplersContainer *samplers;
		Engine::QuadBuffer *quadBuffer;
		//////////////////////////////////////////////////
	};
}
