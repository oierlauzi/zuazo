R""(
#version 140

in vec2             ex_uv;

uniform sampler2D   tex;

uniform data{
    float opacity;
};


void main(){
  gl_FragColor=vec4(texture(tex, ex_uv).rgb, texture(tex, ex_uv).a * opacity);
}
)""
