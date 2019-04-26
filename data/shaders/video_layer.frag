R""(
#version 140

in vec2             ex_uv;

uniform sampler2D   tex;

uniform data{
    float opacity;
};


void main(){
  vec4 texColor=texture(tex, ex_uv);
  float alpha=texture(tex, ex_uv).a * opacity;

  if(alpha == 0.0)
    discard; //This fragment isn't visible

  gl_FragColor=vec4(texColor.rgb, alpha);
}
)""
