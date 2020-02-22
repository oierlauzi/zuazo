#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "ColorTransfer.glsl"

layout(location = 0) in vec4 ex_color;

layout(location = 0) out vec4 out_color;

void main() {
    const ColorTransfer inTr = {
        mat4(1.0),
        COLOR_TRANSFER_FUNCTION_LINEAR,
        COLOR_RANGE_FULL_RGB,
        PLANAR_FORMAT_RGBA
    };
    const ColorTransfer outTr = {
        mat4(1.0),
        COLOR_TRANSFER_FUNCTION_LINEAR,
        COLOR_RANGE_FULL_RGB,
        PLANAR_FORMAT_RGBA
    };

    out_color = ct_transferColor(inTr, outTr, ex_color);
}
 
