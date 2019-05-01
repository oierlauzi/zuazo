#include <Processors/BrightnessContrast.h>

using namespace Zuazo::Processors;

BrightnessContrast::BrightnessContrast() : 
ShaderEffect(std::string(
    #include "../../data/shaders/brightness_contrast.glsl"
)){
    setBrightness(0.5);
    setContrast(0.5);
}

BrightnessContrast::BrightnessContrast(const Utils::VideoMode& vidMode) : 
ShaderEffect(vidMode, std::string(
    #include "../../data/shaders/brightness_contrast.glsl"
)){
    setBrightness(0.5);
    setContrast(0.5);
}