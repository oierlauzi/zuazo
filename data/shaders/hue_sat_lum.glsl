R""(

uniform shaderFxDataBlock{
  float hue;
  float saturation;
  float luminosity;
};

vec3 rgb2hsl(vec3 rgbColor);
vec3 hsl2rgb(vec3 hslColor);

vec4 shaderFx(sampler2D tex, vec2 texCoord){
  vec4 texColor=texture(tex, texCoord);
  vec3 hsl=rgb2hsl(texColor.rgb);

  hsl.x+=hue; //Shift the hue
  hsl.y*=saturation; //Adjust saturation
  hsl.z*=luminosity; //Adjust luminosity

  return vec4(hsl2rgb(hsl), texColor.a);
}

float minOf3(vec3 values){
  return min(values.x, min(values.y, values.z));
}

float maxOf3(vec3 values){
  return max(values.x, max(values.y, values.z));
}

/*
Formulas from:
https://es.wikipedia.org/wiki/Modelo_de_color_HSL
https://en.wikipedia.org/wiki/HSL_and_HSV#HSL_to_RGB
http://alaingalvan.tumblr.com/post/79864187609/glsl-color-correction-shaders
*/

vec3 rgb2hsl(vec3 rgbColor){
  vec3 HSLcolor;
  float minVal=minOf3(rgbColor.rgb);
  float maxVal=maxOf3(rgbColor.rgb);
  float diff=maxVal - minVal;
  float sum=maxVal + minVal; //AKA luminosity times 2

  // The hue component
  if(diff == 0.0){ //minVal == maxVal
    HSLcolor.x=0.0;
  }else if(maxVal == rgbColor.r){
    HSLcolor.x = mod((60.0 * (rgbColor.g - rgbColor.b) / diff + 360.0), 360.0);
  }else if(maxVal == rgbColor.g){
    HSLcolor.x = 60.0 * (rgbColor.b - rgbColor.r) / diff + 120.0;
  }else{ // maxVal == rgbColor.b
    HSLcolor.x = 60.0 * (rgbColor.r - rgbColor.g) / diff + 240.0;
  }

  //The saturation component
  if(diff == 0.0){ //minVal == maxVal
    HSLcolor.y=0.0;
  }else if(sum <= 1.0){
    HSLcolor.y=diff / sum;
  }else{ // sum > 1.0
    HSLcolor.y=diff / (2.0 - sum);
  }

  // The luminosity component
  HSLcolor.z=sum / 2.0;

  return HSLcolor;
}

vec3 hsl2rgb(vec3 hslColor){
  vec3 RGBcolor;
  /*float c=(1.0 - abs(2.0 * hslColor.z - 1.0)) * hslColor.y;
  float x=(1.0 - abs(mod(hslColor.x, 120.0) - 1.0)) * c;
  float m=hslColor.z - c / 2.0;

  if(0.0 <= hslColor.x && hslColor.x < 60.0){
    RGBcolor=vec3(c, x, 0);
  } else if (60.0 <= hslColor.x && hslColor.x < 120.0){
    RGBcolor=vec3(x, c, 0);
  } else if (120.0 <= hslColor.x && hslColor.x < 180.0){
    RGBcolor=vec3(0, c, x);
  } else if (180.0 <= hslColor.x && hslColor.x < 240.0){
    RGBcolor=vec3(0, x, c);
  } else if (240.0 <= hslColor.x && hslColor.x < 300.0){
    RGBcolor=vec3(x, 0, c);
  } else { // 300.0 <= hslColor.x && hslColor.x <= 360.0
    RGBcolor=vec3(c, 0, x);
  }

  RGBcolor+=vec3(m);*/

  float a=hslColor.y * min(hslColor.z, 1.0 - hslColor.z);
  vec3 k=vec3(
    mod(0.0 + hslColor.x / 30.0, 12.0),
    mod(8.0 + hslColor.x / 30.0, 12.0),
    mod(4.0 + hslColor.x / 30.0, 12.0)
  );

  RGBcolor=vec3(
    hslColor.z - a * max(minOf3(vec3(k.r - 3.0, 9.0 - k.r, 1.0)), -1.0),
    hslColor.z - a * max(minOf3(vec3(k.g - 3.0, 9.0 - k.g, 1.0)), -1.0),
    hslColor.z - a * max(minOf3(vec3(k.b - 3.0, 9.0 - k.b, 1.0)), -1.0)
  );

  return RGBcolor;
}

)""
