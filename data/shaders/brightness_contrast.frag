R""(
#version 300 es
precision mediump float;

in vec2             ex_texCoords;

out vec4            out_fragColor;

uniform sampler2D   tex;

uniform shaderFxBlock{
    float contrast;
    float brightness;
};

void main(){
  vec4 texColor=texture(tex, ex_texCoords);
  vec3 color=texColor.rgb * vec3(contrast + 0.5) + vec3(brightness - 0.5);
  out_fragColor=vec4(color, texColor.a);
}
)""
