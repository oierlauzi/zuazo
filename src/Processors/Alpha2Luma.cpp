#include <Processors/Alpha2Luma.h>

using namespace Zuazo::Processors;

const std::string Alpha2Luma::s_shaderSource(
    #include "../../data/shaders/alpha2luma.glsl"
);

Alpha2Luma::Alpha2Luma() : ShaderEffect(s_shaderSource)
{
    setBrightness(0.5);
    setContrast(0.5);
}

Alpha2Luma::Alpha2Luma(const Utils::VideoMode& vidMode) : ShaderEffect(vidMode, s_shaderSource)
{
    setBrightness(0.5);
    setContrast(0.5);
}