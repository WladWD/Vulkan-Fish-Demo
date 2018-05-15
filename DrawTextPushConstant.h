#include "DrawTextLineShaderPipeline.h"

#pragma once
namespace Shader {
	struct DrawTextLineShaderPipeline::PushConstantBuffer {
		glm::vec4 scaleOffset;
		union {
			uint32_t uData[28];
			char cData[112];
		} data;
	};
}

