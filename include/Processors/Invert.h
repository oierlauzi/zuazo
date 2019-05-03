#pragma once

#include "ShaderEffect.h"

namespace Zuazo::Processors{

class Invert : public ShaderEffect{
public:
    Invert();
    Invert(const Utils::VideoMode& vidMode);
    Invert(const Invert& other)=delete;
    Invert(Invert&& other)=default;
    virtual ~Invert()=default;
private:
    static const std::string s_shaderSource;
};
}