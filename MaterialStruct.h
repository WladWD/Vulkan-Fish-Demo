#include "VulkanHeader.h"

#pragma once
namespace Draw {
	struct Material {
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;
		glm::vec4 emissiveColor;

		int32_t diffuseTextureID;
		int32_t specularTextureID;
		int32_t normalTextureID;
		int32_t reflectionTextureID;
	};
}