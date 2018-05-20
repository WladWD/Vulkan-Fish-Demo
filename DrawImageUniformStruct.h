#include "FragmentShaderImageDraw.h"

#pragma once
namespace Shader {
	struct DrawImageShaderPipeline::UniformBuffer {
		glm::mat4 mProjView;
		glm::mat4 mWorld;
	};
}