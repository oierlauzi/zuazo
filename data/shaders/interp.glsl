R""(

#define NEAREST     0
#define BILINEAR    1
#define BICUBIC     2 

uniform scalingFilterBlock{
    int scalingFilter;
};

vec4 nearest(sampler2D near_tex, vec2 texCoord){
    ivec2 texRes=textureSize(near_tex, 0); //Get the size of the texture
    texCoord=(floor(vec2(texRes) * texCoord) + vec2(0.5)) / vec2(texRes); //Set the texture coordenate exactly on top of a sample
    return texture(near_tex, texCoord);
}

vec4 bilinear(sampler2D bil_tex, vec2 texCoord){
    return texture(bil_tex, texCoord); //By default is set up to give us a bilinearly interpolated sample
}

/*
 * This code is based on:
 * http://vec3.ca/bicubic-filtering-in-fewer-taps/
 * http://pastebin.com/raw/YLLSBRFq
 */
vec4 bicubic(sampler2D bicub_tex, vec2 texCoord){
    ivec2 texRes=textureSize(bicub_tex, 0); //Get the size of the texture
    vec2 texelSize=vec2(1.0) / vec2(texRes); //The size of each texel
    vec2 absTexCoord=texCoord * vec2(texRes); //Absolute (non-normalized) texture coordenates

    //Texel components
    vec2 texelCenter=floor(absTexCoord);
    vec2 texelFrac1=absTexCoord - texelCenter;

    //Fractional component's 2nd and 3rd powers
    vec2 texelFrac2 = texelFrac1 * texelFrac1;
    vec2 texelFrac3 = texelFrac2 * texelFrac1;

    //Compute the weights according to B-Spline
    vec2 w0 = texelFrac2 - 0.5 * (texelFrac3 + texelFrac1);
    vec2 w1 = 1.5 * texelFrac3 - 2.5 * texelFrac2 + 1.0;
    vec2 w3 = 0.5 * (texelFrac3 - texelFrac2);
    vec2 w2 = 1.0 - w0 - w1 - w3;

    //Compute the scaling factors
    vec2 s0 = w0 + w1;
    vec2 s1 = w2 + w3;
 
    //Compute the coordenates
    vec2 t0 = texelCenter - 1.0 + w1 / s0;
    vec2 t1 = texelCenter + 1.0 + w3 / s1;

    //Normalize the coordenates
    t0*=texelSize;
    t1*=texelSize;
 
    //and sample and blend
 
    return
        (texture(bicub_tex, vec2(t0.x, t0.y)) * s0.x +
        texture(bicub_tex, vec2(t1.x, t0.y)) * s1.x) * s0.y +
        (texture(bicub_tex, vec2(t0.x, t1.y)) * s0.x +
        texture(bicub_tex, vec2(t1.x, t1.y)) * s1.x) * s1.y;
}

vec4 interp(sampler2D interp_tex, vec2 texCoord){
    vec4 result;

    if( texCoord.x < 0.0 || 
        texCoord.y < 0.0 || 
        texCoord.x >= 1.0 || 
        texCoord.y >= 1.0 )
    {
        result=vec4(0.0);
    } else {
        //Choose a filter
        switch(scalingFilter){
        case (BILINEAR):
            result=bilinear(interp_tex, texCoord);
            break;
        case (BICUBIC):
            result=bicubic(interp_tex, texCoord);
            break;
        default: //(NEAREST)
            result=nearest(interp_tex, texCoord);
            break;
        }
    }

    return result;
}

)""