
vec3 computeNormal(vec3 texNormal, vec3 normal, vec3 tangent) {
	texNormal = texNormal * 2.0f - 1.0f;
	texNormal = normalize(texNormal);

	vec3 normal0 = normalize(normal);
	vec3 tangent0 = normalize(tangent);
	tangent0 = normalize(tangent0 - (dot(tangent0, normal0) * normal0));
	vec3 biNormal0 = cross(tangent0, normal0);

	mat3 tbn = mat3(tangent0, biNormal0, normal0);

	vec3 worldNormal = tbn * texNormal;
	worldNormal = normalize(worldNormal);

	return worldNormal;
}

//layout(early_fragment_tests) in;

//Includes
//#include "utils/utils.glsl"

//layout(set = 1, binding = 1) uniform sampler2D normalTex;
//layout(set = 1, binding = 2) uniform sampler2D specularTex;
//layout(set = 1, binding = 3) uniform sampler2D reflectionTex;