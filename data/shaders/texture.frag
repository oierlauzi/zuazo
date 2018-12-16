#version 130

in vec2             ex_uv;
in vec4             ex_color;

uniform sampler2D   tex;
uniform sampler2D   key;

void main(){
    //Draws picking alpha values from the key and RGB values from the texture
    gl_FragColor=vec4(texture(tex, ex_uv).rgb, texture(key, ex_uv).a) * ex_color;
}
