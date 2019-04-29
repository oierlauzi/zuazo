R""(
#version 300 es
precision mediump float;

in vec2             ex_uv;

out vec4            out_fragColor;

uniform sampler2D   tex;

uniform dataBlock{
    float opacity;
};

)""

#include "interp.glsl"

R""(
void main(){
  vec4 texColor=interp(tex, ex_uv);
  float alpha=texColor.a * opacity;

  if(alpha == 0.0)
    discard; //This fragment isn't visible

  out_fragColor=vec4(texColor.rgb, alpha);
}
)""
