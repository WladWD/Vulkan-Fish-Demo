#version 450 core

#extension GL_ARB_separate_shader_objects: enable
#extension GL_GOOGLE_include_directive: require

//Inputs
layout(location = 0) in vec3 wNormal;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 wEyeDir;

//Uniform
layout(set = 1, binding = 0) uniform sampler2D diffuseTex;

//Push constant
layout (push_constant) uniform PushConstants {
	vec4 diffuseColor;
	vec4 specularColor;
	vec4 emissiveColor;
} color;

//Output
layout(location = 0) out vec4 gOut0;//normal r32g32b32a32  a - shininess(specular power) 
layout(location = 1) out vec4 gOut1;//diffuse r8g8b8a8, a - specular intensity
layout(location = 2) out vec4 gOut2;//specular r8g8b8a8, a - specular power, but spec power = a*255.0f;

void main() {
	vec3 normal = normalize(wNormal);
	vec3 viewDir = normalize(-wEyeDir);
	
	//if (!gl_FrontFacing)normal = -normal;
	vec4 diffuseColor = texture(diffuseTex, texCoord);
	
	gOut0 = vec4(normal, color.specularColor.a);
	gOut1 = vec4(vec3(color.diffuseColor * diffuseColor), color.diffuseColor.a);
	gOut2 = vec4(color.specularColor.rgb, 1.0);
}