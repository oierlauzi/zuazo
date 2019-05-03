R""(

/*         ________________________
 *      __/            |           \_
 * ____/               |             \____
 *     |DECAY|TOLERANCE|
 *          MIN      CENTER     MAX
 */

uniform shaderFxDataBlock{
    float hueCenter;
    float hueTolerance;
    float hueDecay;
    float saturationMin;
    float saturationMax;
    float saturationDecay;
    float luminosityMin;
    float luminosityMax;
    float luminosityDecay;
    float smoothRadius;
};

)""

#include "hsl.glsl"

R""(

#define max(a, b, c) max(a, max(b, c))

float getAttenuation(float x, float minVal, float maxVal, float decay){
    float att;
    if(x < (minVal - decay) || x > (maxVal + decay)){
        //Outside attenuation range
        att=1.0;
    }else if(x < minVal){
        //Decay range on the left
        att=(minVal - x) / decay;
    }else if(x > maxVal){
        //Decay range on the right
        att=(x - maxVal) / decay;
    }else{
        //Full atenuation range
        att=0.0;
    }

    return att;
}

float chromaKey(vec3 inColor){
    vec3 hsl=rgb2hsl(inColor);

    float hueMin=hueCenter - hueTolerance;
    float hueMax=hueCenter + hueTolerance;
    float hueAtt=getAttenuation(hsl.r, hueMin, hueMax, hueDecay);

    float saturationAtt=getAttenuation(hsl.g, saturationMin, saturationMax, saturationDecay);

    float luminosityAtt=getAttenuation(hsl.b, luminosityMin, luminosityMax, luminosityDecay);

    return max(hueAtt, saturationAtt, luminosityAtt);
}

vec4 shaderFx(sampler2D tex, vec2 texCoord){
    vec3 outColor;
    float alpha;

    ivec2 texRes=textureSize(tex, 0); //Get the size of the texture
    vec2 texelSize=vec2(1.0) / vec2(texRes);

    if(smoothRadius > 0.0){
        alpha=1.0;
        int maxX=int(smoothRadius);
        for(int i=-maxX; i <= maxX; ++i){
            int maxY=int(sqrt(smoothRadius*smoothRadius - float(i)*float(i) ));
            for(int j=-maxY; j <= maxY; ++j){
                vec2 pos=vec2(i, j);
                vec3 texelColor=texture(tex, texCoord + pos*texelSize).rgb;

                if(pos == vec2(0.0)){
                    outColor=texelColor;
                }

                alpha*=pow(chromaKey(texelColor), 1.0 - length(pos) / smoothRadius);
            }
        }
    }else{
        outColor=texture(tex, texCoord).rgb;
        alpha=chromaKey(outColor);
    }

    return vec4(outColor, alpha);
}

)""
