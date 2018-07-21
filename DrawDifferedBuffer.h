#include "DrawDifferedShaderPipeline.h"

#pragma once
namespace Shader {
	struct DrawDifferedShaderPipeline::UniformBuffer {
		glm::mat4 mProjView;
		glm::mat4 mWorld;
		glm::mat4 mWorldInvTransp;
		glm::mat4 mTexTransform;
		glm::vec4 cameraPosition;
	};

	struct DrawDifferedShaderPipeline::PushConstantBuffer {
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;
		glm::vec4 emissiveColor;
	};
}