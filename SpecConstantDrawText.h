#include "DrawTextLineShaderPipeline.h"

#pragma once
namespace Shader {
	struct DrawTextLineShaderPipeline::SpecConstantBuffer {
		int row;
		int column;

		static std::array<VkSpecializationMapEntry, 2> getEntry() {
			std::array<VkSpecializationMapEntry, 2> specMapEntry;
			specMapEntry[0].constantID = 0;
			specMapEntry[0].offset = offsetof(DrawTextLineShaderPipeline::SpecConstantBuffer, row);
			specMapEntry[0].size = sizeof(row);

			specMapEntry[1].constantID = 1;
			specMapEntry[1].offset = offsetof(DrawTextLineShaderPipeline::SpecConstantBuffer, column);
			specMapEntry[1].size = sizeof(column);

			return specMapEntry;
		}
	};
}

