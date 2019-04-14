R""(
#version 140

in vec2             ex_texCoords;

uniform sampler2D   tex;

uniform shaderFxBlock{
    float brightness;
    float contrast;
};

void main(){
  vec4 texColor=texture(tex, ex_texCoords);
  vec3 color=texColor.rgb * vec3(contrast + 0.5) + vec3(brightness - 0.5);
  gl_FragColor=vec4(color, texColor.a);
}
)""
