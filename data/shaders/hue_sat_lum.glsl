R""(

uniform shaderFxDataBlock{
  float hue;
  float saturation;
  float luminosity;
};

)""

#include "hsl.glsl"

R""(

vec4 shaderFx(sampler2D tex, vec2 texCoord){
  vec4 texColor=texture(tex, texCoord);
  vec3 hsl=rgb2hsl(texColor.rgb);

  hsl.x+=hue; //Shift the hue
  hsl.y*=saturation; //Adjust saturation
  hsl.z*=luminosity; //Adjust luminosity

  return vec4(hsl2rgb(hsl), texColor.a);
}

)""
