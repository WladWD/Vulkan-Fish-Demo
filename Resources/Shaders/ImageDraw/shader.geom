#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (triangles, invocations = 2) in;
layout (triangle_strip, max_vertices = 6) out;

//Inputs
layout(location = 0) in vec2 fragTexCoord[];
layout(location = 1) in vec3 fPosition[];
layout(location = 2) in vec3 fNormal[];
layout(location = 3) in vec3 fTangent[];

//Output
layout(location = 0) out vec2 rfragTexCoord;
layout(location = 1) out vec3 rfPosition;
layout(location = 2) out vec3 rfNormal;
layout(location = 3) out vec3 rfTangent;

const float xOffset = 2.0;

//Uniform
layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 mProjView;
	mat4 mWorld;
} ubo;

void main() {
	
	for(int i = 0; i < gl_in.length(); ++i){
		rfragTexCoord = fragTexCoord[i];
		rfPosition = fPosition[i];
		rfPosition.y += - 1.0f + xOffset * gl_InvocationID;
		rfPosition.z -= xOffset * gl_InvocationID * 0.5;
		rfNormal = fNormal[i];
		rfTangent = fTangent[i];

		gl_Position = ubo.mProjView * vec4(rfPosition, 1.0);

		EmitVertex();
	}

	EndPrimitive();
}
