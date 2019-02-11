#pragma once

#include "../Utils/PixelTypes.h"
#include "../Stream/Source.h"

namespace Zuazo::Video{

template <Utils::PixelTypes type>
class VideoSource : public Stream::Source{

};
}
