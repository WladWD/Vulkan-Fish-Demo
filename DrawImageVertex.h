#include "VulkanHeader.h"

#pragma once
namespace Shader {
	struct DrawImageVertex {
		glm::vec3 vPosition;
		glm::vec3 vColor;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription(void) {
			VkVertexInputBindingDescription mBindingDescription = {};
			mBindingDescription.binding = 0;
			mBindingDescription.stride = sizeof(DrawImageVertex);
			mBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return mBindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescription(void) {
			std::array<VkVertexInputAttributeDescription, 3> mVertexInputDescription = {};

			mVertexInputDescription[0].binding = 0;
			mVertexInputDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			mVertexInputDescription[0].location = 0;
			mVertexInputDescription[0].offset = offsetof(DrawImageVertex, vPosition);

			mVertexInputDescription[1].binding = 0;
			mVertexInputDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			mVertexInputDescription[1].location = 1;
			mVertexInputDescription[1].offset = offsetof(DrawImageVertex, vColor);

			mVertexInputDescription[2].binding = 0;
			mVertexInputDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputDescription[2].location = 2;
			mVertexInputDescription[2].offset = offsetof(DrawImageVertex, texCoord);

			return mVertexInputDescription;
		}
	};
}
