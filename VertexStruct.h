#include "VulkanHeader.h"
#pragma once

namespace Draw {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	};

	struct VertexTangent: Vertex {
		glm::vec3 tangent;
	};
}
