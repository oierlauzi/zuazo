R""(

uniform shaderFxDataBlock{
    float contrast;
    float brightness;
};

vec4 shaderFx(sampler2D tex, vec2 texCoord){
  vec4 texColor=texture(tex, texCoord);
  vec3 color=texColor.rgb * vec3(contrast + 0.5) + vec3(brightness - 0.5);
  return vec4(color, texColor.a);
}
)""
