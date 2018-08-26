#version 450 core
#extension GL_ARB_separate_shader_objects : enable

//Inputs
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTex;

//Outputs
layout(location = 0) out vec3 wNormal;
layout(location = 1) out vec2 texCoord;
layout(location = 2) out vec3 wEyeDir;

//Uniforms
layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 mProjView;
	mat4 mWorld;
	mat4 mWorldInvTransp;
	mat4 mTexTransform;
	vec4 cameraPosition;
} uniformBuffer;

void main() {
	vec3 wPosition = vec3(uniformBuffer.mWorld * vec4(vPosition, 1.0));

	wNormal = vec3(uniformBuffer.mWorldInvTransp * vec4(vNormal, 0.0));
	texCoord = vec2(uniformBuffer.mTexTransform * vec4(vTex, 1.0, 0.0));
	wEyeDir = wPosition - vec3(uniformBuffer.cameraPosition);

	gl_Position = uniformBuffer.mProjView * vec4(wPosition, 1.0);
}