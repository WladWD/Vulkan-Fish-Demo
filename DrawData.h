#include "ModelBuffer.h"
#include "MaterialStruct.h"

#pragma once
namespace Draw {
	enum class DrawDataTypes {
		DrawDataDiffuse,
		DrawDataDiffuseNormal,
		DrawDataTesselation,
		DrawDataDiffuseSpecular,
		DrawDataDiffuseNormalSpecular,
		DrawDataTesselationSpecular,
		DrawDataPBR,
		DrawDataScinAnim,

		DrawDataTypeCount
	};

	struct DrawData {
		std::string drawDataName;

		std::vector<uint32_t> vertexOffset;
		std::vector<uint32_t> firstIndex;
		std::vector<uint32_t> indexCount;
		std::vector<uint32_t> materialIdx;
	};

	struct Model {
		std::unique_ptr<ModelBuffer> buffer;
		std::vector<Material> materials;
		DrawData drawData[static_cast<uint32_t>(DrawDataTypes::DrawDataTypeCount)];

		glm::mat4 mWorld;
	};
}