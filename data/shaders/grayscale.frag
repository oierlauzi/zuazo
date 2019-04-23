R""(
#version 140

in vec2             ex_texCoords;

uniform sampler2D   tex;

uniform shaderFxBlock{
    float contrast;
    float brightness;
};

void main(){
  const vec3 lummaCoef=vec3(0.2126, 0.7152, 0.0722);
  vec4 texColor=texture(tex, ex_texCoords);
  float lumma=dot(texColor.rgb, lummaCoef) * (contrast + 0.5) + brightness - 0.5;

  gl_FragColor=vec4(lumma, lumma, lumma, texColor.a);
}
)""
