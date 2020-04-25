#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "color_transfer.glsl"

layout(location = 0) in vec2 ex_texCoord;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = ct_SAMPLER_BINDING) uniform sampler2D samplers[ct_SAMPLER_COUNT];
layout(set = 0, binding = ct_DATA_BINDING) uniform InputColorTransferBlock{
	ct_data inColorTransfer;
};
/*layout(set = 1, binding = 0) uniform OutputColorTransferBlock{
	ct_data outColorTransfer;
};*/


void main() {
	ct_data outColorTransfer = inColorTransfer;

	out_color = ct_getColor(inColorTransfer, 
							outColorTransfer, 
							samplers, 
							ex_texCoord );
}
 
