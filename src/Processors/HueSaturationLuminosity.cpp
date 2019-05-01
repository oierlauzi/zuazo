#include <Processors/HueSaturationLuminosity.h>

using namespace Zuazo::Processors;

HueSaturationLuminosisty::HueSaturationLuminosisty() : 
ShaderEffect(std::string(
    #include "../../data/shaders/hue_sat_lum.glsl"
)){
    setHue(0.0);
    setSaturation(1.0);
    setLuminosity(1.0);
}

HueSaturationLuminosisty::HueSaturationLuminosisty(const Utils::VideoMode& vidMode) : 
ShaderEffect(vidMode, std::string(
    #include "../../data/shaders/hue_sat_lum.glsl"
)){
    setHue(0.0);
    setSaturation(1.0);
    setLuminosity(1.0);
}