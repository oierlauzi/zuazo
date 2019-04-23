#include <Processors/Invert.h>

using namespace Zuazo::Processors;

Invert::Invert() : 
ShaderEffect(std::string(
    #include "../../data/shaders/invert.frag"
)){
}

Invert::Invert(const Utils::VideoMode& vidMode) : 
ShaderEffect(vidMode, std::string(
    #include "../../data/shaders/invert.frag"
)){
}