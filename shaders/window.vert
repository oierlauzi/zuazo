#version 450

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texCoord;

layout(location = 0) out vec2 ex_texCoord;
layout(set = 0, binding = 0) uniform ViewPortBlock {
	vec2 viewPortSize;
};


void main() {
    gl_Position = vec4(in_position / viewPortSize, 0.0, 1.0);
	ex_texCoord = in_texCoord;
}