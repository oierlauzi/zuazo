#include <zuazo/Video.h>

namespace Zuazo {

VideoMode makeVideoMode(ColorPrimaries prim, ColorModel model, ColorTransferFunction trf) {
	return VideoMode(
		Utils::Any<Rate>(),
		Utils::Any<Resolution>(),
		Utils::Any<AspectRatio>(),
		Utils::MustBe<ColorPrimaries>(prim),
		Utils::MustBe<ColorModel>(model),
		Utils::MustBe<ColorTransferFunction>(trf),
		Utils::Any<ColorSubsampling>(),
		Utils::Any<ColorRange>(),
		Utils::Any<ColorFormat>()
	);
}

namespace ColorSpaces {

//ITU-R
const VideoMode BT601_625 = makeVideoMode(ColorPrimaries::BT601_625, ColorModel::BT601, ColorTransferFunction::BT1886);
const VideoMode BT601_525 = makeVideoMode(ColorPrimaries::BT601_525, ColorModel::BT601, ColorTransferFunction::BT1886);
const VideoMode BT709 = makeVideoMode(ColorPrimaries::BT709, ColorModel::BT709, ColorTransferFunction::BT1886);
const VideoMode BT2020 = makeVideoMode(ColorPrimaries::BT2020, ColorModel::BT2020, ColorTransferFunction::BT1886);

//Others
const VideoMode SRGB = makeVideoMode(ColorPrimaries::BT709, ColorModel::RGB, ColorTransferFunction::IEC61966_2_1);
const VideoMode DISPLAY_P3 = makeVideoMode(ColorPrimaries::SMPTE432, ColorModel::RGB, ColorTransferFunction::IEC61966_2_1);

}

}