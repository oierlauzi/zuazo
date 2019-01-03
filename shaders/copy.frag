R""(
#version 130

in vec2             ex_uv;

uniform sampler2D   tex;

void main(){
    gl_FragColor=texture(tex, ex_uv).rgba;
}
)""
