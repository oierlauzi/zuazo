#pragma once

#include "ShaderEffect.h"

namespace Zuazo::Processors{

class Luma2Alpha : public ShaderEffect{
public:
    Luma2Alpha();
    Luma2Alpha(const Utils::VideoMode& vidMode);
    Luma2Alpha(const Luma2Alpha& other)=delete;
    Luma2Alpha(Luma2Alpha&& other)=default;
    virtual ~Luma2Alpha()=default;

    void    setBrightness(float brightness);
    float   getBrightness() const;   

    void    setContrast(float contrast);
    float   getContrast() const;   
};

inline void Luma2Alpha::setBrightness(float brightness){
    ShaderEffect::setParam("brightness", brightness);
}

inline float Luma2Alpha::getBrightness() const{
    const float* brightness=ShaderEffect::getParam<float>("brightness");
    return brightness ? *brightness : 0.0;
}   

inline void Luma2Alpha::setContrast(float contrast){
    ShaderEffect::setParam("contrast", contrast);
}

inline float Luma2Alpha::getContrast() const{
    const float* contrast=ShaderEffect::getParam<float>("contrast");
    return contrast ? *contrast : 0.0;
} 

}