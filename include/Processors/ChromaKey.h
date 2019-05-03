#pragma once

#include "ShaderEffect.h"

namespace Zuazo::Processors{

class ChromaKey : public ShaderEffect{
public:
    ChromaKey();
    ChromaKey(const Utils::VideoMode& vidMode);
    ChromaKey(const ChromaKey& other)=delete;
    ChromaKey(ChromaKey&& other)=default;
    virtual ~ChromaKey()=default;

    void    setHue(float hue);
    float   getHue() const;   

    void    setHueTolerance(float tol);
    float   getHueTolerance() const; 

    void    setHueDecay(float decay);
    float   getHueDecay() const;


    void    setMinimumSaturation(float sat);
    float   getMinimumSaturation() const;   

    void    setMaximumSaturation(float sat);
    float   getMaximumSaturation() const;   

    void    setSaturationDecay(float decay);
    float   getSaturationDecay() const;


    void    setMinimumLuminosity(float lum);
    float   getMinimumLuminosity() const;   

    void    setMaximumLuminosity(float lum);
    float   getMaximumLuminosity() const;   

    void    setLuminosityDecay(float decay);
    float   getLuminosityDecay() const;
private:
    static const std::string s_shaderSource;
};

inline void ChromaKey::setHue(float hue){
    ShaderEffect::setParam("hueCenter", hue);
}

inline float ChromaKey::getHue() const{
    const float* ptr=ShaderEffect::getParam<float>("hueCenter");
    return ptr ? *ptr : 0.0;
}   


inline void ChromaKey::setHueTolerance(float tol){
    ShaderEffect::setParam("hueTolerance", tol);
}

inline float ChromaKey::getHueTolerance() const{
    const float* ptr=ShaderEffect::getParam<float>("hueTolerance");
    return ptr ? *ptr : 0.0;
}


inline void ChromaKey::setHueDecay(float dec){
    ShaderEffect::setParam("hueDecay", dec);
}

inline float ChromaKey::getHueDecay() const{
    const float* ptr=ShaderEffect::getParam<float>("hueDecay");
    return ptr ? *ptr : 0.0;
}   





inline void ChromaKey::setMinimumSaturation(float sat){
    ShaderEffect::setParam("saturationMin", sat);
}

inline float ChromaKey::getMinimumSaturation() const{
    const float* ptr=ShaderEffect::getParam<float>("saturationMin");
    return ptr ? *ptr : 0.0;
}   


inline void ChromaKey::setMaximumSaturation(float sat){
    ShaderEffect::setParam("saturationMax", sat);
}

inline float ChromaKey::getMaximumSaturation() const{
    const float* ptr=ShaderEffect::getParam<float>("saturationMax");
    return ptr ? *ptr : 0.0;
}


inline void ChromaKey::setSaturationDecay(float dec){
    ShaderEffect::setParam("saturationDecay", dec);
}

inline float ChromaKey::getSaturationDecay() const{
    const float* ptr=ShaderEffect::getParam<float>("saturationDecay");
    return ptr ? *ptr : 0.0;
}   






inline void ChromaKey::setMinimumLuminosity(float sat){
    ShaderEffect::setParam("luminosityMin", sat);
}

inline float ChromaKey::getMinimumLuminosity() const{
    const float* ptr=ShaderEffect::getParam<float>("luminosityMin");
    return ptr ? *ptr : 0.0;
}   


inline void ChromaKey::setMaximumLuminosity(float sat){
    ShaderEffect::setParam("luminosityMax", sat);
}

inline float ChromaKey::getMaximumLuminosity() const{
    const float* ptr=ShaderEffect::getParam<float>("luminosityMax");
    return ptr ? *ptr : 0.0;
}


inline void ChromaKey::setLuminosityDecay(float dec){
    ShaderEffect::setParam("luminosityDecay", dec);
}

inline float ChromaKey::getLuminosityDecay() const{
    const float* ptr=ShaderEffect::getParam<float>("luminosityDecay");
    return ptr ? *ptr : 0.0;
}   
}