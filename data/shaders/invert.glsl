R""(

vec4 shaderFx(sampler2D tex, vec2 texCoord){
    vec4 texColor=texture(tex, ex_texCoords);
    vec3 fragColor=vec3(1.0) - texColor.rgb;
    return vec4(fragColor, 1.0);
}

)""