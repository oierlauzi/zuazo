R""(

uniform shaderFxDataBlock{
    float contrast;
    float brightness;
};

vec4 shaderFx(sampler2D tex, vec2 texCoord){
  const vec3 lummaCoef=vec3(0.2126, 0.7152, 0.0722);
  vec4 texColor=texture(tex, texCoord);
  float lumma=dot(texColor.rgb, lummaCoef) * (contrast + 0.5) + brightness - 0.5;

  return vec4(lumma, lumma, lumma, texColor.a);
}

)""
