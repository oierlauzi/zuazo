#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "ColorTransfer.glsl"

layout(location = 0) in vec2 ex_texCoord;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D samplers[IMAGE_COUNT];
layout(set = 0, binding = 1) uniform ColorTransferBlock{
	ColorTransfer inColorTransfer;
};


void main() {
	const ColorTransfer outTr = {
		mat4(1.0),
		mat4(1.0),
		COLOR_TRANSFER_FUNCTION_LINEAR,
		COLOR_RANGE_FULL_RGB,
		PLANE_FORMAT_RGBA
	};

	out_color = ct_getColor(inColorTransfer, outTr, samplers, ex_texCoord);
}
 
