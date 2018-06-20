#version 450
#extension GL_ARB_separate_shader_objects : enable
layout (vertices = 4) out;
/////////////////////////////////////////////////////////////////////////////////////
layout(location = 0) in vec2 fragTexCoord[];
layout(location = 1) in vec3 fNormal[];
layout(location = 2) in vec3 fTangent[];
/////////////////////////////////////////////////////////////////////////////////////
layout(location = 0) out vec2 teTexCoord[];
layout(location = 1) out vec3 teNormal[];
layout(location = 2) out vec3 teTangent[];
/////////////////////////////////////////////////////////////////////////////////////
layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 mProjView;
	mat4 mWorld;
} ubo;
/////////////////////////////////////////////////////////////////////////////////////
void main() {
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	teTexCoord[gl_InvocationID] = fragTexCoord[gl_InvocationID];
	teNormal[gl_InvocationID] = fNormal[gl_InvocationID];
	teTangent[gl_InvocationID] = fTangent[gl_InvocationID];

	if(gl_InvocationID == 0) {

		float s = 1.0;//64.0;

		gl_TessLevelInner[0] = s;
		gl_TessLevelInner[1] = s;

		gl_TessLevelOuter[0] = s;
		gl_TessLevelOuter[1] = s;
		gl_TessLevelOuter[2] = s;
		gl_TessLevelOuter[3] = s;
	}
}

	