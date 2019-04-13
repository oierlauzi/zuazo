#include <Processors/Grayscale.h>

using namespace Zuazo::Processors;

Grayscale::Grayscale() : 
ShaderEffect(std::string(
    #include "../../data/shaders/grayscale.frag"
)){
    setGain(1.0);
}

Grayscale::Grayscale(const Utils::VideoMode& vidMode) : 
ShaderEffect(vidMode, std::string(
    #include "../../data/shaders/grayscale.frag"
)){
    setGain(1.0);
}