#include <Processors/Grayscale.h>

using namespace Zuazo::Processors;

const std::string Grayscale::s_shaderSource(
    #include "../../data/shaders/grayscale.glsl"
);

Grayscale::Grayscale() : ShaderEffect(s_shaderSource)
{
    setBrightness(0.5);
    setContrast(0.5);
}

Grayscale::Grayscale(const Utils::VideoMode& vidMode) : ShaderEffect(vidMode, s_shaderSource)
{
    setBrightness(0.5);
    setContrast(0.5);
}