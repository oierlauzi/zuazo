#version 450

layout(location = 0) out vec4 ex_color;

void main() {
	const vec2 positions[] = {
		vec2(-1.0, -1.0),
		vec2(-1.0, +1.0),
		vec2(+1.0, -1.0),
		vec2(+1.0, +1.0)
	};

	const vec4 colors[] = {
		vec4(1.0f, 0.0f, 0.0f, 1.0f),
		vec4(0.0f, 1.0f, 0.0f, 1.0f),
		vec4(0.0f, 0.0f, 1.0f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f)
	};

    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	ex_color = colors[gl_VertexIndex];
}