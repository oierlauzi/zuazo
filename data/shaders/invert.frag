R""(
#version 140

in vec2             ex_texCoords;

uniform sampler2D   tex;

void main(){
    vec4 texColor=texture(tex, ex_texCoords);
    vec3 fragColor=vec3(1.0) - texColor.rgb;
    gl_FragColor=vec4(fragColor, 1.0);
}
)""