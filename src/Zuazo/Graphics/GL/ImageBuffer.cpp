#include "ImageBuffer.h"

namespace Zuazo::Graphics::GL{

const std::map<PixelFormats, size_t> pixelComponentsMap={
		{PixelFormats::NONE,		0		},
		{PixelFormats::RED,			1		},
		{PixelFormats::GREEN,		1		},
		{PixelFormats::BLUE,		1		},
		{PixelFormats::ALPHA,		1		},
		{PixelFormats::RGB,			3		},
		{PixelFormats::BGR,			3		},
		{PixelFormats::RGBA,		4		},
		{PixelFormats::BGRA,		4		},
};

const std::map<PixelFormats, Utils::PixelFormats> pixelConversionsMap={
		{PixelFormats::NONE,		Utils::PixelFormats::NONE		},
		{PixelFormats::RED,			Utils::PixelFormats::GRAY8		},
		{PixelFormats::GREEN,		Utils::PixelFormats::GRAY8		},
		{PixelFormats::BLUE,		Utils::PixelFormats::GRAY8		},
		{PixelFormats::ALPHA,		Utils::PixelFormats::GRAY8		},
		{PixelFormats::RGB,			Utils::PixelFormats::RGB24		},
		{PixelFormats::BGR,			Utils::PixelFormats::BGR24		},
		{PixelFormats::RGBA,		Utils::PixelFormats::RGBA		},
		{PixelFormats::BGRA,		Utils::PixelFormats::BGRA		},
};
}

