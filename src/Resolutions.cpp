#include <zuazo/Video.h>

namespace Zuazo {

VideoMode makeVideoMode(Resolution res, AspectRatio par) {
	return VideoMode(
		Utils::Any<Rate>(),
		Utils::MustBe<Resolution>(res),
		Utils::MustBe<AspectRatio>(par),
		Utils::Any<ColorPrimaries>(),
		Utils::Any<ColorModel>(),
		Utils::Any<ColorTransferFunction>(),
		Utils::Any<ColorSubsampling>(),
		Utils::Any<ColorRange>(),
		Utils::Any<ColorFormat>()
	);
}


namespace Resolutions {

//Broadcast-standard
const VideoMode PAL = makeVideoMode(Resolution(720, 576), AspectRatio(786, 720));
const VideoMode NTSC = makeVideoMode(Resolution(720, 480), AspectRatio(654, 720));

const VideoMode HD = makeVideoMode(Resolution(1280, 720), AspectRatio(1, 1));
const VideoMode FHD = makeVideoMode(Resolution(1920, 1080), AspectRatio(1, 1));
const VideoMode QHD = makeVideoMode(Resolution(2560, 1440), AspectRatio(1, 1));
const VideoMode UHD = makeVideoMode(Resolution(3840, 2160), AspectRatio(1, 1));
const VideoMode UHD_2 = makeVideoMode(Resolution(7680, 4320), AspectRatio(1, 1));

//DCI (Digital cinema Initiatives)
const VideoMode DCI_2K = makeVideoMode(Resolution(2048, 1080), AspectRatio(1, 1));
const VideoMode DCI_4K = makeVideoMode(Resolution(4096, 2160), AspectRatio(1, 1));
const VideoMode DCI_8K = makeVideoMode(Resolution(8192, 4320), AspectRatio(1, 1));

//VGA (Video graphics array)
const VideoMode QQVGA = makeVideoMode(Resolution(160, 120), AspectRatio(1, 1));
const VideoMode HQVGA = makeVideoMode(Resolution(256, 160), AspectRatio(1, 1));
const VideoMode QVGA = makeVideoMode(Resolution(320, 240), AspectRatio(1, 1));
const VideoMode WQVGA = makeVideoMode(Resolution(384, 240), AspectRatio(1, 1));
const VideoMode HVGA = makeVideoMode(Resolution(480, 320), AspectRatio(1, 1));
const VideoMode VGA = makeVideoMode(Resolution(640, 480), AspectRatio(1, 1));
const VideoMode WVGA = makeVideoMode(Resolution(768, 480), AspectRatio(1, 1));
const VideoMode FWVGA = makeVideoMode(Resolution(854, 480), AspectRatio(1, 1));
const VideoMode SVGA = makeVideoMode(Resolution(800, 600), AspectRatio(1, 1));
const VideoMode WSVGA = makeVideoMode(Resolution(1024, 576), AspectRatio(1, 1));
const VideoMode DVGA = makeVideoMode(Resolution(960, 640), AspectRatio(1, 1));

//XGA (Extended graphics array)
const VideoMode XGA = makeVideoMode(Resolution(1024, 768), AspectRatio(1, 1));
const VideoMode WXGA = makeVideoMode(Resolution(1152, 768), AspectRatio(1, 1));
const VideoMode FWXGA = makeVideoMode(Resolution(1366, 768), AspectRatio(1, 1));
const VideoMode SXGA = makeVideoMode(Resolution(1280, 1024), AspectRatio(1, 1));
const VideoMode WSXGA = makeVideoMode(Resolution(1440, 960), AspectRatio(1, 1));
const VideoMode UXGA = makeVideoMode(Resolution(1600, 1200), AspectRatio(1, 1));
const VideoMode WUXGA = makeVideoMode(Resolution(1920, 1200), AspectRatio(1, 1)); 

//QXGA (Quad Extended Graphics Array)
const VideoMode QWXGA = makeVideoMode(Resolution(2048, 1152), AspectRatio(1, 1));
const VideoMode QXGA = makeVideoMode(Resolution(2048, 1536), AspectRatio(1, 1));
const VideoMode WQXGA = makeVideoMode(Resolution(2560, 1600), AspectRatio(1, 1));
const VideoMode QSXGA = makeVideoMode(Resolution(2560, 2048), AspectRatio(1, 1));
const VideoMode WQSXGA = makeVideoMode(Resolution(3200, 2048), AspectRatio(1, 1));
const VideoMode QUXGA = makeVideoMode(Resolution(3200, 2400), AspectRatio(1, 1));
const VideoMode WQUXGA = makeVideoMode(Resolution(3840, 2400), AspectRatio(1, 1));

}

}