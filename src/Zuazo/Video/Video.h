#pragma once

#include "../Utils/PixelTypes.h"
#include "../Stream/Source.h"
#include "../Graphics/Frame.h"

namespace Zuazo::Video{

template<Graphics::FrameTypes type>
using VideoSource=Stream::Source< Graphics::Frame<type> >;

template<Graphics::FrameTypes type>
using VideoConsumer=Stream::Consumer< Graphics::Frame<type> >; //TODO specialize for cross-cases

}
