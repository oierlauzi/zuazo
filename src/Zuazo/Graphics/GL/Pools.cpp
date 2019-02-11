#include "Pool.h"
#include "Texture.h"
#include "PixelIO.h"
#include "../../Utils/ImageAttributes.h"

namespace Zuazo::Graphics::GL{

MultiPool<Utils::ImageAttributes, Texture> texturePool;
MultiPool<size_t, PixelUnpackBuffer> pboPool;

}
