#version 450
#extension GL_ARB_separate_shader_objects : enable
//////////////////////////////////////////////////////////////////////////
//Inputs
layout(location = 0) in vec2 texCoord;
//////////////////////////////////////////////////////////////////////////
//Uniform
layout(binding = 0) uniform sampler2D texChar;
//////////////////////////////////////////////////////////////////////////
//Specialization Const
const int row = 16;
const int column = 16;
//////////////////////////////////////////////////////////////////////////
//Push constant
layout (push_constant) uniform push_constants_t {
	vec4 scaleOffset;
	uint uData[28];
} push_constants;
//////////////////////////////////////////////////////////////////////////
//Output
layout(location = 0) out vec4 gOut;

int getByte(in uint value, int offset) {
	return int(value >> (offset * 8) & 0x000000ff);
}

void main() {
	float d = texCoord.x * float(getByte(push_constants.uData[0], 0));
	int dig = int(floor(d));
	vec2 tex = vec2(mod(d, 1.0), texCoord.y);
	int letter = getByte(push_constants.uData[(dig + 1) >> 2], (dig + 1) % 4);
	vec2 scale = vec2(1.0 / float(column), 1.0 / float(row));
	tex *= scale;
	tex += scale * vec2(float(letter % column), float(letter / column));

	gOut = texture(texChar, tex);
	//if(gOut.r < 0.1) discard;
}
