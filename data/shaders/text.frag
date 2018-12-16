#version 130

in vec2             ex_uv;

uniform sampler2D   tex;

void main(){
    gl_FragColor=vec4(1.0, 1.0, 1.0, texture(tex, ex_uv).r);
}
