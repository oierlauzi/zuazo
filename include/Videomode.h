#pragma once

#include "PixelFormat.h"
#include "Resolution.h"
#include "Math/Rational.h"
#include "Utils/Hasher.h"

#include <set>
#include <memory>

namespace Zuazo {

struct Videomode{
    PixelFormat			pixelFormat;
	Resolution			resolution;
    Math::Rational_t	frameRate;
};

ZUAZO_DEFAULT_HASH(Videomode);

}