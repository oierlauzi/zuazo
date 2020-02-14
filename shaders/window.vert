#version 450

layout(location = 0) out vec2 texCoords;

void main() {
	const vec2 positions[] = {
		vec2(0.0, 0.0),
		vec2(0.0, 1.0),
		vec2(1.0, 0.0),
		vec2(1.0, 1.0)
	};

    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	texCoords = positions[gl_VertexIndex];
}