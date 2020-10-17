#include <zuazo/Video.h>

namespace Zuazo {

VideoMode makeVideoMode(Rate rate) {
	return VideoMode(
		Utils::MustBe<Rate>(rate),
		Utils::Any<Resolution>(),
		Utils::Any<AspectRatio>(),
		Utils::Any<ColorPrimaries>(),
		Utils::Any<ColorModel>(),
		Utils::Any<ColorTransferFunction>(),
		Utils::Any<ColorSubsampling>(),
		Utils::Any<ColorRange>(),
		Utils::Any<ColorFormat>()
	);
}

namespace FrameRates {

constexpr Rate NTSC_FACTOR(1000, 1001);

const VideoMode P24 = makeVideoMode(Rate(24, 1));
const VideoMode P48 = makeVideoMode(Rate(48, 1));
const VideoMode P96 = makeVideoMode(Rate(96, 1));
const VideoMode P192 = makeVideoMode(Rate(192, 1));

const VideoMode P25 = makeVideoMode(Rate(25, 1));
const VideoMode P50 = makeVideoMode(Rate(50, 1));
const VideoMode P100 = makeVideoMode(Rate(100, 1));
const VideoMode P200 = makeVideoMode(Rate(200, 1));

const VideoMode P29_97 = makeVideoMode(Rate(30, 1) * NTSC_FACTOR);
const VideoMode P59_94 = makeVideoMode(Rate(60, 1) * NTSC_FACTOR);
const VideoMode P119_88 = makeVideoMode(Rate(120, 1) * NTSC_FACTOR);
const VideoMode P239_76 = makeVideoMode(Rate(240, 1) * NTSC_FACTOR);

const VideoMode P30 = makeVideoMode(Rate(30, 1));
const VideoMode P60 = makeVideoMode(Rate(60, 1));
const VideoMode P120 = makeVideoMode(Rate(120, 1));
const VideoMode P240 = makeVideoMode(Rate(240, 1));

}

}