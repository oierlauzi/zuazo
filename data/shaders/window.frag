R""(
#version 300 es
precision mediump float;

in vec2             ex_uv;
out vec4            out_fragColor;

uniform sampler2D   tex;

)""

#include "interp.glsl"

R""(
void main(){
    out_fragColor=interp(tex, ex_uv);
}
)""
