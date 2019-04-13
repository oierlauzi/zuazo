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

    void    setGain(float gain);
    float   getGain() const;   
};

inline void Grayscale::setGain(float gain){
    ShaderEffect::setParam("gain", gain);
}

inline float Grayscale::getGain() const{
    const float* gain=ShaderEffect::getParam<float>("gain");
    return gain ? *gain : 0.0;
}   

}