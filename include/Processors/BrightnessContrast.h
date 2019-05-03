#pragma once

#include "ShaderEffect.h"

namespace Zuazo::Processors{

class BrightnessContrast : public ShaderEffect{
public:
    BrightnessContrast();
    BrightnessContrast(const Utils::VideoMode& vidMode);
    BrightnessContrast(const BrightnessContrast& other)=delete;
    BrightnessContrast(BrightnessContrast&& other)=default;
    virtual ~BrightnessContrast()=default;

    void    setBrightness(float brightness);
    float   getBrightness() const;   

    void    setContrast(float contrast);
    float   getContrast() const;   
private:
    static const std::string s_shaderSource;
};

inline void BrightnessContrast::setBrightness(float brightness){
    ShaderEffect::setParam("brightness", brightness);
}

inline float BrightnessContrast::getBrightness() const{
    const float* brightness=ShaderEffect::getParam<float>("brightness");
    return brightness ? *brightness : 0.0;
}   

inline void BrightnessContrast::setContrast(float contrast){
    ShaderEffect::setParam("contrast", contrast);
}

inline float BrightnessContrast::getContrast() const{
    const float* contrast=ShaderEffect::getParam<float>("contrast");
    return contrast ? *contrast : 0.0;
} 

}