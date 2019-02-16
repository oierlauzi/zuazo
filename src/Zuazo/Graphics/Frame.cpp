#include "Frame.h"

#include <stddef.h>

#include "../Utils/ImageAttributes.h"
#include "GL/Buffers/PixelUnpackBuffer.h"

namespace Zuazo::Graphics{

MultiPool<size_t, GL::Buffers::PixelUnpackBuffer> Frame::pboPool;
MultiPool<Utils::ImageAttributes, GL::Texture> 	Frame::texPool;

}
