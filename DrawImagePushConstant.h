#include "DrawImageShaderPipeline.h"

#pragma once
namespace Shader {
	struct DrawImageShaderPipeline::PushConstantBuffer {
		glm::vec4 lightColor;
		glm::vec3 lightPosition;
		float pad0;
		glm::vec3 camPos;
		float pad1;
	};
}

