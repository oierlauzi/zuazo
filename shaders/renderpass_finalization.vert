#version 450

//Vertex I/O


//Uniform buffers


void main() {
	const ivec2 BIT_OFFSET = ivec2(2, 1);
	const ivec2 BIT_MASK = ivec2(4);

	//x: (gl_VertexIndex & 1) ? +3.0f : -1.0f
	//y: (gl_VertexIndex & 2) ? +3.0f : -1.0f
	gl_Position = vec4(
		vec2((ivec2(gl_VertexIndex) << BIT_OFFSET) & BIT_MASK) - vec2(1.0f),
		0.0f, 1.0f
	);
}
