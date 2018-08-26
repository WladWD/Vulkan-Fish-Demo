#include "VulkanHeader.h"
#pragma once

namespace Draw {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription(void) {
			VkVertexInputBindingDescription mBindingDescription = {};
			mBindingDescription.binding = 0;
			mBindingDescription.stride = sizeof(Vertex);
			mBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return mBindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescription(void) {
			std::array<VkVertexInputAttributeDescription, 3> mVertexInputDescription = {};

			mVertexInputDescription[0].binding = 0;
			mVertexInputDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			mVertexInputDescription[0].location = 0;
			mVertexInputDescription[0].offset = offsetof(Vertex, position);

			mVertexInputDescription[1].binding = 0;
			mVertexInputDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			mVertexInputDescription[1].location = 1;
			mVertexInputDescription[1].offset = offsetof(Vertex, normal);

			mVertexInputDescription[2].binding = 0;
			mVertexInputDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputDescription[2].location = 2;
			mVertexInputDescription[2].offset = offsetof(Vertex, texCoord);

			return mVertexInputDescription;
		}
	};

	struct VertexTangent {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
		glm::vec3 tangent;

		static VkVertexInputBindingDescription getBindingDescription(void) {
			VkVertexInputBindingDescription mBindingDescription = {};
			mBindingDescription.binding = 0;
			mBindingDescription.stride = sizeof(VertexTangent);
			mBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return mBindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescription(void) {
			std::array<VkVertexInputAttributeDescription, 4> mVertexInputDescription = {};

			mVertexInputDescription[0].binding = 0;
			mVertexInputDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			mVertexInputDescription[0].location = 0;
			mVertexInputDescription[0].offset = offsetof(VertexTangent, position);

			mVertexInputDescription[1].binding = 0;
			mVertexInputDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			mVertexInputDescription[1].location = 1;
			mVertexInputDescription[1].offset = offsetof(VertexTangent, normal);

			mVertexInputDescription[2].binding = 0;
			mVertexInputDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputDescription[2].location = 2;
			mVertexInputDescription[2].offset = offsetof(VertexTangent, texCoord);

			mVertexInputDescription[3].binding = 0;
			mVertexInputDescription[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			mVertexInputDescription[3].location = 3;
			mVertexInputDescription[3].offset = offsetof(VertexTangent, tangent);

			return mVertexInputDescription;
		}
	};
}
