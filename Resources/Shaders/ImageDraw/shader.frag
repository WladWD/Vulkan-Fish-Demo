#version 450
#extension GL_ARB_separate_shader_objects : enable
//////////////////////////////////////////////////////////////////////////
//Inputs
layout(location = 0) in vec3 g_FragColor;
layout(location = 1) in vec2 fragTexCoord;
//////////////////////////////////////////////////////////////////////////
//Uniform
layout(binding = 1) uniform sampler2D texSampler;
//////////////////////////////////////////////////////////////////////////
//Spec Constant
//layout (constant_id = 1) const vec4 color = vec4(1.0, 1.0, 1.0, 1.0); 
//////////////////////////////////////////////////////////////////////////
//Push constant
layout (push_constant) uniform push_constants_t {
	vec4 multipleColor;
} push_constants;
//////////////////////////////////////////////////////////////////////////
//Output
layout(location = 0) out vec4 g_FragmentResultColor;
void main() {
	g_FragmentResultColor = texture(texSampler, fragTexCoord) * push_constants.multipleColor;
}
