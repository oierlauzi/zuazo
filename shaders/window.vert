#version 450

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texCoord;

layout(location = 0) out vec2 ex_texCoord;

void main() {
	const vec2 positions[] = {
		vec2(-1.0, -1.0),
		vec2(-1.0, +1.0),
		vec2(+1.0, -1.0),
		vec2(+1.0, +1.0)
	};

	const vec2 colors[] = {
		vec2(0.0f, 0.0f ),
		vec2(0.0f, 1.0f ),
		vec2(1.0f, 0.0f ),
		vec2(1.0f, 1.0f )
	};

    gl_Position = vec4(in_position, 0.0, 1.0);
	ex_texCoord = in_texCoord;
}