#include <Processors/Alpha2Luma.h>

using namespace Zuazo::Processors;

Alpha2Luma::Alpha2Luma() : 
ShaderEffect(std::string(
    #include "../../data/shaders/alpha2luma.glsl"
)){
    setBrightness(0.5);
    setContrast(0.5);
}

Alpha2Luma::Alpha2Luma(const Utils::VideoMode& vidMode) : 
ShaderEffect(vidMode, std::string(
    #include "../../data/shaders/alpha2luma.glsl"
)){
    setBrightness(0.5);
    setContrast(0.5);
}