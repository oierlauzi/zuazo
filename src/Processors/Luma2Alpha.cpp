#include <Processors/Luma2Alpha.h>

using namespace Zuazo::Processors;

Luma2Alpha::Luma2Alpha() : 
ShaderEffect(std::string(
    #include "../../data/shaders/luma2alpha.frag"
)){
    setBrightness(0.5);
    setContrast(0.5);
}

Luma2Alpha::Luma2Alpha(const Utils::VideoMode& vidMode) : 
ShaderEffect(vidMode, std::string(
    #include "../../data/shaders/luma2alpha.frag"
)){
    setBrightness(0.5);
    setContrast(0.5);
}