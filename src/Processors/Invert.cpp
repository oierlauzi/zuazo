#include <Processors/Invert.h>

using namespace Zuazo::Processors;

Invert::Invert() : 
ShaderEffect(std::string(
    #include "../../data/shaders/invert.glsl"
)){
}

Invert::Invert(const Utils::VideoMode& vidMode) : 
ShaderEffect(vidMode, std::string(
    #include "../../data/shaders/invert.glsl"
)){
}