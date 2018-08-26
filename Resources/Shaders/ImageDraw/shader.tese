#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(quads, equal_spacing) in;
/////////////////////////////////////////////////////////////////////////////////////
layout(location = 0) in vec2 teTexCoord[];
layout(location = 1) in vec3 teNormal[];
layout(location = 2) in vec3 teTangent[];
/////////////////////////////////////////////////////////////////////////////////////
layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fPosition;
layout(location = 2) out vec3 fNormal;
layout(location = 3) out vec3 fTangent;
/////////////////////////////////////////////////////////////////////////////////////
layout(set = 0, binding = 3) uniform sampler2D dispMap;
layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 mProjView;
	mat4 mWorld;
} ubo;
/////////////////////////////////////////////////////////////////////////////////////

vec2 sampled2(vec2 a, vec2 b, vec2 c, vec2 d) {
	vec2 m0 = mix(a, b, gl_TessCoord.x);
	vec2 m1 = mix(c, d, gl_TessCoord.x);
	return mix(m0, m1, gl_TessCoord.y);
}

vec3 sampled3(vec3 a, vec3 b, vec3 c, vec3 d) {
	vec3 m0 = mix(a, b, gl_TessCoord.x);
	vec3 m1 = mix(c, d, gl_TessCoord.x);
	return mix(m0, m1, gl_TessCoord.y);
}

vec4 sampled4(vec4 a, vec4 b, vec4 c, vec4 d) {
	vec4 m0 = mix(a, b, gl_TessCoord.x);
	vec4 m1 = mix(c, d, gl_TessCoord.x);
	return mix(m0, m1, gl_TessCoord.y);
}

void main() {
	fragTexCoord = sampled2(teTexCoord[0], teTexCoord[1], teTexCoord[2], teTexCoord[3]);
	fNormal = sampled3(teNormal[0], teNormal[1], teNormal[2], teNormal[3]);
	fTangent = sampled3(teTangent[0], teTangent[1], teTangent[2], teTangent[3]);

	vec4 pos = sampled4(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
	//pos.z -= texture(dispMap, fragTexCoord).r * 0.2;
	gl_Position = ubo.mProjView * pos;

	fPosition = pos.xyz;
}

	