#pragma once

#include "ShaderEffect.h"

namespace Zuazo::Processors{

class HueSaturationLuminosity : public ShaderEffect{
public:
    HueSaturationLuminosity();
    HueSaturationLuminosity(const Utils::VideoMode& vidMode);
    HueSaturationLuminosity(const HueSaturationLuminosity& other)=delete;
    HueSaturationLuminosity(HueSaturationLuminosity&& other)=default;
    virtual ~HueSaturationLuminosity()=default;

    void    setHue(float hue);
    float   getHue() const;   

    void    setSaturation(float sat);
    float   getSaturation() const;   

    void    setLuminosity(float lum);
    float   getLuminosity() const; 
private:
    static const std::string s_shaderSource;
};

inline void HueSaturationLuminosity::setHue(float hue){
    ShaderEffect::setParam("hue", hue);
}

inline float HueSaturationLuminosity::getHue() const{
    const float* hue=ShaderEffect::getParam<float>("hue");
    return hue ? *hue : 0.0;
}   

inline void HueSaturationLuminosity::setSaturation(float sat){
    ShaderEffect::setParam("saturation", sat);
}

inline float HueSaturationLuminosity::getSaturation() const{
    const float* sat=ShaderEffect::getParam<float>("saturation");
    return sat ? *sat : 0.0;
}   

inline void HueSaturationLuminosity::setLuminosity(float lum){
    ShaderEffect::setParam("luminosity", lum);
}

inline float HueSaturationLuminosity::getLuminosity() const{
    const float* lum=ShaderEffect::getParam<float>("luminosity");
    return lum ? *lum : 0.0;
}   

}