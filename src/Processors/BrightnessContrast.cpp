#include <Processors/BrightnessContrast.h>

using namespace Zuazo::Processors;

const std::string BrightnessContrast::s_shaderSource(
    #include "../../data/shaders/brightness_contrast.glsl"
);

BrightnessContrast::BrightnessContrast() : ShaderEffect(s_shaderSource)
{
    setBrightness(0.5);
    setContrast(0.5);
}

BrightnessContrast::BrightnessContrast(const Utils::VideoMode& vidMode) : ShaderEffect(vidMode, s_shaderSource)
{
    setBrightness(0.5);
    setContrast(0.5);
}