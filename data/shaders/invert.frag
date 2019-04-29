R""(
#version 300 es
precision mediump float;

in vec2             ex_texCoords;

out vec4            out_fragColor;

uniform sampler2D   tex;

void main(){
    vec4 texColor=texture(tex, ex_texCoords);
    vec3 fragColor=vec3(1.0) - texColor.rgb;
    out_fragColor=vec4(fragColor, 1.0);
}
)""