#pragma once

#include "ShaderEffect.h"

namespace Zuazo::Processors{

class HueSaturationLuminosisty : public ShaderEffect{
public:
    HueSaturationLuminosisty();
    HueSaturationLuminosisty(const Utils::VideoMode& vidMode);
    HueSaturationLuminosisty(const HueSaturationLuminosisty& other)=delete;
    HueSaturationLuminosisty(HueSaturationLuminosisty&& other)=default;
    virtual ~HueSaturationLuminosisty()=default;

    void    setHue(float hue);
    float   getHue() const;   

    void    setSaturation(float sat);
    float   getSaturation() const;   

    void    setLuminosity(float lum);
    float   getLuminosity() const; 
private:
    static const std::string s_shaderSource;
};

inline void HueSaturationLuminosisty::setHue(float hue){
    ShaderEffect::setParam("hue", hue);
}

inline float HueSaturationLuminosisty::getHue() const{
    const float* hue=ShaderEffect::getParam<float>("hue");
    return hue ? *hue : 0.0;
}   

inline void HueSaturationLuminosisty::setSaturation(float sat){
    ShaderEffect::setParam("saturation", sat);
}

inline float HueSaturationLuminosisty::getSaturation() const{
    const float* sat=ShaderEffect::getParam<float>("saturation");
    return sat ? *sat : 0.0;
}   

inline void HueSaturationLuminosisty::setLuminosity(float lum){
    ShaderEffect::setParam("luminosity", lum);
}

inline float HueSaturationLuminosisty::getLuminosity() const{
    const float* lum=ShaderEffect::getParam<float>("luminosity");
    return lum ? *lum : 0.0;
}   

}