#include <Processors/Grayscale.h>

using namespace Zuazo::Processors;

Grayscale::Grayscale() : 
ShaderEffect(std::string(
    #include "../../data/shaders/grayscale.glsl"
)){
    setBrightness(0.5);
    setContrast(0.5);
}

Grayscale::Grayscale(const Utils::VideoMode& vidMode) : 
ShaderEffect(vidMode, std::string(
    #include "../../data/shaders/grayscale.glsl"
)){
    setBrightness(0.5);
    setContrast(0.5);
}