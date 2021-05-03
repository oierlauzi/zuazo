#version 450

/*
 * This shader needs to be executed for 3
 * vertices so that a triangle larger than
 * the viewport is emitted. Therefore, the
 * FS will be executed for ALL the pixels
 * on the FB. 
 */

//Vertex I/O
layout(location = 0) out vec2 out_uv;

//Uniform buffers


void main() {
	const ivec2 BIT_MASK = ivec2(1, 2); //Selects the first and second bits

	//x: (gl_VertexIndex & 1) ? +3.0f : -1.0f
	//y: (gl_VertexIndex & 2) ? +3.0f : -1.0f
	const vec2 pos = mix(vec2(-1), vec2(+3), ivec2(gl_VertexIndex) & BIT_MASK);
	const vec2 uv = 0.5f*(pos + vec2(1.0f));

	gl_Position = vec4(pos, 0.0f, 1.0f);
	out_uv = uv;
}
