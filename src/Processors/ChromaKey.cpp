#include <Processors/ChromaKey.h>

using namespace Zuazo::Processors;

const std::string ChromaKey::s_shaderSource(
    #include "../../data/shaders/chroma_key.glsl"
);

ChromaKey::ChromaKey() : ShaderEffect(s_shaderSource)
{
    setHue(110.0); //Green
    setHueTolerance(30.0);
    setHueDecay(10.0);

    setMinimumSaturation(0.6);
    setMaximumSaturation(1.0);
    setSaturationDecay(0.1);

    setMinimumLuminosity(0.3);
    setMaximumLuminosity(1.0);
    setLuminosityDecay(0.1);
}

ChromaKey::ChromaKey(const Utils::VideoMode& vidMode) : ShaderEffect(vidMode, s_shaderSource)
{
    setHue(110.0); //Green
    setHueTolerance(30.0);
    setHueDecay(10.0);

    setMinimumSaturation(0.6);
    setMaximumSaturation(1.0);
    setSaturationDecay(0.2);

    setMinimumLuminosity(0.3);
    setMaximumLuminosity(0.7);
    setLuminosityDecay(0.1);
}