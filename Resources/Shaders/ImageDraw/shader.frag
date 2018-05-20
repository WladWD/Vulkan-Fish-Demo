#version 450
#extension GL_ARB_separate_shader_objects : enable
//////////////////////////////////////////////////////////////////////////
//Inputs
layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fPosition;
layout(location = 2) in vec3 fNormal;
layout(location = 3) in vec3 fTangent;
//////////////////////////////////////////////////////////////////////////
//Uniform
layout(set = 0, binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform sampler2D normalMap;
//////////////////////////////////////////////////////////////////////////
//Spec Constant
//layout (constant_id = 1) const vec4 color = vec4(1.0, 1.0, 1.0, 1.0); 
//////////////////////////////////////////////////////////////////////////
//Push constant
layout (push_constant) uniform push_constants_t {
	vec4 lightColor;
	vec3 lightPosition;
} push_constants;
//////////////////////////////////////////////////////////////////////////
//Output
layout(location = 0) out vec4 g_FragmentResultColor;
//////////////////////////////////////////////////////////////////////////

float colorize(in vec3 pos, in vec3 normal, in vec3 lightPosition) {
	vec3 dir = (lightPosition - pos);
	float d = length(dir);
	dir *= (1.0 / d);
	normal = normalize(normal);
	float diff = max(dot(dir, normal), 0.0);
	return diff / (d * d * 0.1);
}

vec3 getNormal() {
	vec3 normal = normalize(fNormal);
	vec3 tangent = fTangent - dot(fTangent, fNormal) * fNormal;
	vec3 biTangent = cross(normal, tangent);

	vec3 norm = texture(normalMap, fragTexCoord).xyz;
	norm = norm * 2.0 - 1.0;
	norm = mat3(tangent, biTangent, normal) * norm;
	norm = normalize(norm);
	return norm;
}

void main() {
	g_FragmentResultColor = texture(texSampler, fragTexCoord) * push_constants.lightColor * colorize(fPosition, getNormal(), push_constants.lightPosition);
}
