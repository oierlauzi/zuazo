#include <Processors/HueSaturationLuminosity.h>

using namespace Zuazo::Processors;

const std::string HueSaturationLuminosisty::s_shaderSource(
    #include "../../data/shaders/hue_sat_lum.glsl"
);

HueSaturationLuminosisty::HueSaturationLuminosisty() : ShaderEffect(s_shaderSource)
{
    setHue(0.0);
    setSaturation(1.0);
    setLuminosity(1.0);
}

HueSaturationLuminosisty::HueSaturationLuminosisty(const Utils::VideoMode& vidMode) : ShaderEffect(vidMode, s_shaderSource)
{
    setHue(0.0);
    setSaturation(1.0);
    setLuminosity(1.0);
}