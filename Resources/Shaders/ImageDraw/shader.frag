#version 450
#extension GL_ARB_separate_shader_objects : enable

//Inputs
layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fPosition;
layout(location = 2) in vec3 fNormal;
layout(location = 3) in vec3 fTangent;

//Uniform
layout(set = 0, binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform sampler2D normalMap;
layout(set = 0, binding = 3) uniform sampler2D dispMap;

//Uniform buffer
layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 mProjView;
	mat4 mWorld;
} ubo;

//Spec Constant
//layout (constant_id = 1) const vec4 color = vec4(1.0, 1.0, 1.0, 1.0); 

//Push constant
layout (push_constant) uniform push_constants_t {
	vec4 lightColor;
	vec3 lightPosition;
	vec3 camPos;
} push_constants;


//Output
layout(location = 0) out vec4 g_FragmentResultColor;

float colorize(in vec3 pos, in vec3 normal, in vec3 lightPosition) {
	vec3 dir = (lightPosition - pos);
	float d = length(dir);
	dir *= (1.0 / d);
	normal = normalize(normal);
	float diff = max(dot(dir, normal), 0.0);
	return 0.1 + diff / (d * d * 0.1 + d * 0.5);
}

vec3 getNormal(vec2 coord) {
	vec3 normal = normalize(fNormal);
	vec3 tangent = normalize(fTangent - dot(fTangent, fNormal) * fNormal);
	vec3 biTangent = cross(normal, tangent);

	vec3 norm = texture(normalMap, coord).xyz;
	norm = norm * 2.0 - 1.0;
	norm = mat3(tangent, biTangent, normal) * norm;
	norm = normalize(norm);
	return norm;
}

vec2 computeParalax(vec2 coord, vec3 cameraPos, vec3 fragPos) {
	float h = texture(dispMap, coord).r;

	vec3 normal = normalize(fNormal);
	vec3 tangent = normalize(fTangent - dot(fTangent, fNormal) * fNormal);
	vec3 biTangent = cross(normal, tangent);
	
	mat3 tbn = transpose(mat3(tangent, biTangent, normal));

	vec3 tgCamPos = tbn * cameraPos;
	vec3 tgFragPos = tbn * fragPos;
	vec3 tgCamDir = normalize(tgCamPos - tgFragPos);

	vec2 offset = tgCamDir.xy / tgCamDir.z * (h * 0.1);

	return coord - offset;
}

void main() {
	
	vec3 cameraPosition = push_constants.camPos;//vec3(0.0, 1.0, 2.0);
	vec2 fragCoord = computeParalax(fragTexCoord, cameraPosition, fPosition);

	//vec3 norm = getNormal(fragCoord);
	//norm = norm * 0.5 + 0.5;
	g_FragmentResultColor = texture(texSampler, fragCoord) * push_constants.lightColor * colorize(fPosition, getNormal(fragCoord), push_constants.lightPosition);
}
