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
const VideoMode BT601_625 = makeVideoMode(ColorPrimaries::bt601_625, ColorModel::bt601, ColorTransferFunction::bt1886);
const VideoMode BT601_525 = makeVideoMode(ColorPrimaries::bt601_525, ColorModel::bt601, ColorTransferFunction::bt1886);
const VideoMode BT709 = makeVideoMode(ColorPrimaries::bt709, ColorModel::bt709, ColorTransferFunction::bt1886);
const VideoMode BT2020 = makeVideoMode(ColorPrimaries::bt2020, ColorModel::bt2020, ColorTransferFunction::bt1886);

//Others
const VideoMode SRGB = makeVideoMode(ColorPrimaries::bt709, ColorModel::rgb, ColorTransferFunction::iec61966_2_1);
const VideoMode DISPLAY_P3 = makeVideoMode(ColorPrimaries::smpte432, ColorModel::rgb, ColorTransferFunction::iec61966_2_1);

}

}