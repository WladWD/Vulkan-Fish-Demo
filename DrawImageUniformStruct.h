#include "FragmentShaderImageDraw.h"

#pragma once
namespace Shader {
	struct DrawImageShaderPipeline::UniformBuffer {
		glm::mat4 projView;
	};
}