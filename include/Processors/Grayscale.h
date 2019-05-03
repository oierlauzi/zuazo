#pragma once

#include "ShaderEffect.h"

namespace Zuazo::Processors{

class Grayscale : public ShaderEffect{
public:
    Grayscale();
    Grayscale(const Utils::VideoMode& vidMode);
    Grayscale(const Grayscale& other)=delete;
    Grayscale(Grayscale&& other)=default;
    virtual ~Grayscale()=default;

    void    setBrightness(float brightness);
    float   getBrightness() const;   

    void    setContrast(float contrast);
    float   getContrast() const;   
private:
    static const std::string s_shaderSource;
};

inline void Grayscale::setBrightness(float brightness){
    ShaderEffect::setParam("brightness", brightness);
}

inline float Grayscale::getBrightness() const{
    const float* brightness=ShaderEffect::getParam<float>("brightness");
    return brightness ? *brightness : 0.0;
}   

inline void Grayscale::setContrast(float contrast){
    ShaderEffect::setParam("contrast", contrast);
}

inline float Grayscale::getContrast() const{
    const float* contrast=ShaderEffect::getParam<float>("contrast");
    return contrast ? *contrast : 0.0;
} 

}