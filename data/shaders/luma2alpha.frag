R""(
#version 140

in vec2             ex_texCoords;

uniform sampler2D   tex;

uniform shaderFxBlock{
    float contrast;
    float brightness;
};

void main(){
  const vec4 lummaCoef=vec4(0.2126, 0.7152, 0.0722, 0.0);
  const vec3 white=vec3(1.0);
  
  vec4 texColor=texture(tex, ex_texCoords);
  float lumma=dot(texColor, lummaCoef) * (contrast + 0.5) + brightness - 0.5;

  gl_FragColor=vec4(white, lumma);
}
)""
