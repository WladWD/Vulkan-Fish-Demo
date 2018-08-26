#include "VulkanHeader.h"
#pragma once
namespace VertexFormat {
	struct VertexPosition {
		glm::vec3 position;

		static VkVertexInputBindingDescription getBindingDescription(void) {
			VkVertexInputBindingDescription mBindingDescription = {};
			mBindingDescription.binding = 0;
			mBindingDescription.stride = sizeof(VertexPosition);
			mBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return mBindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescription(void) {
			std::array<VkVertexInputAttributeDescription, 1> mVertexInputDescription = {};

			mVertexInputDescription[0].binding = 0;
			mVertexInputDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			mVertexInputDescription[0].location = 0;
			mVertexInputDescription[0].offset = offsetof(VertexPosition, position);

			return mVertexInputDescription;
		}
	};

	struct VertexPositionTexSingleBuffer {
		glm::vec3 position;
		glm::vec2 tex;

		static VkVertexInputBindingDescription getBindingDescription(void) {
			VkVertexInputBindingDescription mBindingDescription = {};
			mBindingDescription.binding = 0;
			mBindingDescription.stride = sizeof(VertexPositionTexSingleBuffer);
			mBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return mBindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescription(void) {
			std::array<VkVertexInputAttributeDescription, 2> mVertexInputDescription = {};

			mVertexInputDescription[0].binding = 0;
			mVertexInputDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			mVertexInputDescription[0].location = 0;
			mVertexInputDescription[0].offset = offsetof(VertexPositionTexSingleBuffer, position);

			mVertexInputDescription[1].binding = 0;
			mVertexInputDescription[1].format = VK_FORMAT_R32G32_SFLOAT;
			mVertexInputDescription[1].location = 1;
			mVertexInputDescription[1].offset = offsetof(VertexPositionTexSingleBuffer, tex);

			return mVertexInputDescription;
		}
	};

	struct VertexPositionNormal {
		glm::vec3 position;
		glm::vec3 normal;
	};

	struct VertexPositionNormalTex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 tex;
	};
}
