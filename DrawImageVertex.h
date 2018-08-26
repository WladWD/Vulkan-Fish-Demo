#include "VulkanHeader.h"

#pragma once
namespace Shader {
	struct DrawImageVertex {
		glm::vec3 vPosition;
		glm::vec3 vColor;
		glm::vec2 texCoord;
		glm::vec3 normal;
		glm::vec3 tangent;

		static VkVertexInputBindingDescription getBindingDescription(void) {
			VkVertexInputBindingDescription mBindingDescription = {};
			mBindingDescription.binding = 0;
			mBindingDescription.stride = sizeof(DrawImageVertex);
			mBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return mBindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescription(void) {
			std::array<VkVertexInputAttributeDescription, 5> mVertexInputDescription = {};

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

			mVertexInputDescription[3].binding = 0;
			mVertexInputDescription[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			mVertexInputDescription[3].location = 3;
			mVertexInputDescription[3].offset = offsetof(DrawImageVertex, normal);

			mVertexInputDescription[4].binding = 0;
			mVertexInputDescription[4].format = VK_FORMAT_R32G32B32_SFLOAT;
			mVertexInputDescription[4].location = 4;
			mVertexInputDescription[4].offset = offsetof(DrawImageVertex, tangent);

			return mVertexInputDescription;
		}
	};
}
