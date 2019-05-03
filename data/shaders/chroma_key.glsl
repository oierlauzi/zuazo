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

vec4 shaderFx(sampler2D tex, vec2 texCoord){
    vec4 texColor=texture(tex, texCoord);
    vec3 hsl=rgb2hsl(texColor.rgb);

    float hueMin=hueCenter - hueTolerance;
    float hueMax=hueCenter + hueTolerance;
    float hueAtt=getAttenuation(hsl.r, hueMin, hueMax, hueDecay);

    float saturationAtt=getAttenuation(hsl.g, saturationMin, saturationMax, saturationDecay);

    float luminosityAtt=getAttenuation(hsl.b, luminosityMin, luminosityMax, luminosityDecay);

    float alpha=max(hueAtt, saturationAtt, luminosityAtt);

    return vec4(hsl2rgb(hsl), alpha);
}

)""
