#include <zuazo/Video.h>

namespace Zuazo {

VideoMode makeVideoMode(ColorSubsampling subs, ColorRange range, ColorFormat format) {
	return VideoMode(
		Utils::Any<Rate>(),
		Utils::Any<Resolution>(),
		Utils::Any<AspectRatio>(),
		Utils::Any<ColorPrimaries>(),
		Utils::Any<ColorModel>(),
		Utils::Any<ColorTransferFunction>(),
		Utils::MustBe<ColorSubsampling>(subs),
		Utils::MustBe<ColorRange>(range),
		Utils::MustBe<ColorFormat>(format)
	);
}

namespace PixelFormats {

//Render optimal
const VideoMode RENDER_OPTIMAL_8 = makeVideoMode(ColorSubsampling::RB_444, ColorRange::FULL, ColorFormat::R8G8B8A8);
const VideoMode RENDER_OPTIMAL_16 = makeVideoMode(ColorSubsampling::RB_444, ColorRange::FULL, ColorFormat::R16G16B16A16);
const VideoMode RENDER_OPTIMAL_16F = makeVideoMode(ColorSubsampling::RB_444, ColorRange::FULL, ColorFormat::R16fG16fB16fA16f);

}

}