#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "color_transfer.glsl"

layout(location = 0) in vec2 ex_texCoord;

layout(location = 0) out vec4 out_color;


layout(set = 0, binding = 1) uniform OutputColorTransferBlock{
	ct_data outColorTransfer;
};

layout(set = 1, binding = ct_SAMPLER_BINDING) uniform sampler2D samplers[ct_SAMPLER_COUNT];
layout(set = 1, binding = ct_DATA_BINDING) uniform InputColorTransferBlock {
	ct_data inColorTransfer;
};

void main() {
	vec4 read = ct_getColor(inColorTransfer, 
							outColorTransfer, 
							samplers, 
							ex_texCoord );

	out_color = ct_writeColor(outColorTransfer, read);
}
 
