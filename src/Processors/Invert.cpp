#include <Processors/Invert.h>

using namespace Zuazo::Processors;

const std::string Invert::s_shaderSource(
    #include "../../data/shaders/invert.glsl"
);

Invert::Invert() : ShaderEffect(s_shaderSource)
{
}

Invert::Invert(const Utils::VideoMode& vidMode) : ShaderEffect(vidMode, s_shaderSource)
{
}