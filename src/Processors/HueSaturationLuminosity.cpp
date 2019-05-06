#include <Processors/HueSaturationLuminosity.h>

using namespace Zuazo::Processors;

const std::string HueSaturationLuminosity::s_shaderSource(
    #include "../../data/shaders/hue_sat_lum.glsl"
);

HueSaturationLuminosity::HueSaturationLuminosity() : ShaderEffect(s_shaderSource)
{
    setHue(0.0);
    setSaturation(1.0);
    setLuminosity(1.0);
}

HueSaturationLuminosity::HueSaturationLuminosity(const Utils::VideoMode& vidMode) : ShaderEffect(vidMode, s_shaderSource)
{
    setHue(0.0);
    setSaturation(1.0);
    setLuminosity(1.0);
}