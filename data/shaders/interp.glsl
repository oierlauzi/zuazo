R""(

#define NEAREST     0
#define BILINEAR    1
#define BICUBIC     2 

uniform scalingFilterBlock{
    int scalingFilter;
};

vec4 nearest(sampler2D near_tex, vec2 texCoord){
    return texture(near_tex, texCoord);
}

/*
 * This code is based on:
 * https://www.codeproject.com/Articles/236394/Bi-Cubic-and-Bi-Linear-Interpolation-with-GLSL#GLSLLinear
 */

vec4 bilinear(sampler2D bil_tex, vec2 texCoord){
    ivec2 texRes=textureSize(bil_tex, 0); //Get the size of the texture
    vec2 texelSize=vec2(1.0) / vec2(texRes); //The size of each texel

    //Calulate the fractional part of the texture coordenates
    float fracX = fract( texCoord.x * float(texRes.x) );
    float fracY = fract( texCoord.y * float(texRes.y) );

    mat4 samples=mat4(
        texture2D(bil_tex, texCoord + vec2(0.0,         0.0         )), //Columns!
        texture2D(bil_tex, texCoord + vec2(texelSize.x, 0.0         )),
        texture2D(bil_tex, texCoord + vec2(0.0,         texelSize.y )),
        texture2D(bil_tex, texCoord + texelSize                      )
    );

    vec4 weights=vec4(
        (1.0 - fracX) * (1.0 - fracY),
        fracX         * (1.0 - fracY),
        (1.0 - fracX) * fracY,
        fracX         * fracY
    );

    vec4 interpolatedColor=samples * weights;

    return interpolatedColor;
}

float triang(float x){
    return 1.0 - abs(x) / 2.0;
}

float triang(vec2 vector){
    return triang(vector.x) * triang(vector.y);
}

float bspline(float x){
    x=abs(x);
    
    if( x >= 0.0 && x <= 1.0 ){
		return ( 2.0 / 3.0 ) +  0.5  * ( x * x * x ) - (x * x);
	}else if( x > 1.0 && x <= 2.0 ){
        float a=2.0 - x;
		return 1.0 / 6.0 * a * a * a;
    }else{
        return 1.0;
    }
}

float bspline(vec2 vector){
    return bspline(vector.x) * bspline(vector.y);
}

/*
 * This code is based on:
 * https://www.codeproject.com/Articles/236394/Bi-Cubic-and-Bi-Linear-Interpolation-with-GLSL#GLSLLinear
 */

vec4 bicubic(sampler2D bicub_tex, vec2 texCoord){
    ivec2 texRes=textureSize(bicub_tex, 0); //Get the size of the texture
    vec2 texelSize=vec2(1.0) / vec2(texRes); //The size of each texel

    vec4 numSum = vec4(0.0);
    vec4 denSum = vec4(0.0);

    //Calulate the fractional part of the texture coordenates
    float fracX = fract( texCoord.x * float(texRes.x) );
    float fracY = fract( texCoord.y * float(texRes.y) );
    vec2 texCoordFrac=vec2(fracX, fracY);

    //Sum all the terms
    for(int i=-1; i < 3; ++i){
        for(int j=-1; j < 3; ++j){
            //Sample the texture
            vec2 textureSampleCoord=vec2(i, j);
			vec4 textureSample = texture2D(bicub_tex, texCoord + texelSize * textureSampleCoord);

            //Sample a triangular window
            vec2 windowCoord=textureSampleCoord - texCoordFrac;
            float windowSample=bspline(windowCoord);

            //Add the values to the sum
            numSum+=textureSample * windowSample;
            denSum+=windowSample;
        }
    }

    return numSum / denSum; 
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