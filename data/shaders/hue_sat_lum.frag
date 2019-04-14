R""(
#version 140

in vec2             ex_texCoords;

uniform sampler2D   tex;

uniform shaderFxBlock{
  float hue;
  float sat;
  float val;
};

vec3 rgb2hsl(vec3 rgbColor);
vec3 hsl2rgb(vec3 rgbColor);

void main(){
  gl_FragColor=vec4(lumma, lumma, lumma, texture(tex, ex_texCoords).a);
}

float minOf3(vec3 values){
  min(values.x, min(values.y, values.z));
}

float maxOf3(vec3 values){
  max(values.x, max(values.y, values.z));
}

vec3 rgb2hsl(vec3 rgbColor){
  vec3 HSLcolor;
  float minVal=minOf3(rgbColor.rgb);
  float maxVal=maxOf3(rgbColor.rgb);

  if(minVal == maxVal){
    HSLcolor.x=0;
  }else if(maxVal == rgbColor.r){

  }else if(maxVal == rgbColor.g){

  }else{ // maxVal == rgbColor.b

  }
}

vec3 hsl2rgb(vec3 rgbColor){

}

)""
