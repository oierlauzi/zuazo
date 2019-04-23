#pragma once

#include "ShaderEffect.h"

namespace Zuazo::Processors{

class Alpha2Luma : public ShaderEffect{
public:
    Alpha2Luma();
    Alpha2Luma(const Utils::VideoMode& vidMode);
    Alpha2Luma(const Alpha2Luma& other)=delete;
    Alpha2Luma(Alpha2Luma&& other)=default;
    virtual ~Alpha2Luma()=default;

    void    setBrightness(float brightness);
    float   getBrightness() const;   

    void    setContrast(float contrast);
    float   getContrast() const;   
};

inline void Alpha2Luma::setBrightness(float brightness){
    ShaderEffect::setParam("brightness", brightness);
}

inline float Alpha2Luma::getBrightness() const{
    const float* brightness=ShaderEffect::getParam<float>("brightness");
    return brightness ? *brightness : 0.0;
}   

inline void Alpha2Luma::setContrast(float contrast){
    ShaderEffect::setParam("contrast", contrast);
}

inline float Alpha2Luma::getContrast() const{
    const float* contrast=ShaderEffect::getParam<float>("contrast");
    return contrast ? *contrast : 0.0;
} 

}