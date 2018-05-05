#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 g_FragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 g_FragmentResultColor;
void main() {
	g_FragmentResultColor = texture(texSampler, fragTexCoord);
}
