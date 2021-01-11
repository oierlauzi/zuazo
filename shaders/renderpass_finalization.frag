#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "color_transfer.glsl"

//Vertex I/O
layout(location = 0) out vec4 out_color0;
layout(location = 1) out vec4 out_color1;
layout(location = 2) out vec4 out_color2;
layout(location = 3) out vec4 out_color3;

//Uniform buffers
layout(set = 0, binding = 0) uniform OutputColorTransferBlock{
	ct_write_data outColorTransfer;
};

//Input attachments
layout (input_attachment_index = 0, binding = 1) uniform subpassInput in_color;


void main() {
	vec4 color = subpassLoad(in_color);

	//Convert it
	ct_output(
		outColorTransfer, color,
		out_color0, out_color1, out_color2, out_color3			
	);
}
 
