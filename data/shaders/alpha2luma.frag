R""(
#version 140

in vec2             ex_texCoords;

uniform sampler2D   tex;

uniform shaderFxBlock{
    float contrast;
    float brightness;
};

void main(){
    vec4 texColor=texture(tex, ex_texCoords);
    vec3 fragColor=vec3(texColor.a) * vec3(contrast + 0.5) + vec3(brightness - 0.5);
    gl_FragColor=vec4(fragColor, 1.0);
}
)""