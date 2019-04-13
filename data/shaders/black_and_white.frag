R""(
#version 140

in vec2             ex_texCoords;

uniform sampler2D   tex;

void main(){
  const vec4 lummaCoef=vec4(0.2126, 0.7152, 0.0722, 0.0);
  float lumma=dot(texture(tex, ex_texCoords), lummaCoef);

  gl_FragColor=vec4(lumma, lumma, lumma, texture(tex, ex_texCoords).a);
}
)""
