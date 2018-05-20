#version 450
#extension GL_ARB_separate_shader_objects : enable
/////////////////////////////////////////////////////////////////////////////////////
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;
/////////////////////////////////////////////////////////////////////////////////////
//Constant
layout (constant_id = 0) const float scale = 3.0; 
/////////////////////////////////////////////////////////////////////////////////////
layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fNormal;
layout(location = 2) out vec3 fTangent;
/////////////////////////////////////////////////////////////////////////////////////
layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 mProjView;
	mat4 mWorld;
} ubo;
/////////////////////////////////////////////////////////////////////////////////////

void main() {
	vec4 pos = ubo.mWorld * vec4(vPosition * scale, 1.0);
	gl_Position = pos;//ubo.mProjView * pos;
	//g_FragColor = vColor;
	//fPosition = pos.xyz;


	mat3 tr = mat3(transpose(inverse(ubo.mWorld)));
	fNormal = tr * normal;
	fTangent = tr * tangent;
	fragTexCoord = inTexCoord;
}

	