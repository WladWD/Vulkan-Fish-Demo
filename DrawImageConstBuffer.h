#include "DrawImageShaderPipeline.h"

#pragma once
namespace Shader {
	struct DrawImageShaderPipeline::ConstBuffer {
		float scale;
		glm::vec4 color;

		static std::array<VkSpecializationMapEntry, 2> getEntry() {
			std::array<VkSpecializationMapEntry, 2> specMapEntry;
			specMapEntry[0].constantID = 0;
			specMapEntry[0].offset = offsetof(DrawImageShaderPipeline::ConstBuffer, scale);
			specMapEntry[0].size = sizeof(scale);

			specMapEntry[1].constantID = 1;
			specMapEntry[1].offset = offsetof(DrawImageShaderPipeline::ConstBuffer, color);
			specMapEntry[1].size = sizeof(color);

			return specMapEntry;
		}
	};
}

