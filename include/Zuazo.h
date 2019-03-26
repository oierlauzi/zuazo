#pragma once

#include <memory>
#include <string>

namespace Zuazo {
namespace Video {
class VideoOutputBase;
} /* namespace Video */
} /* namespace Zuazo */

namespace Zuazo::Timing {
class Timings;
} /* namespace Zuazo */

namespace Zuazo{

//Errors that initialization and terminating can produce
enum class Errors{
    OK			=0,
    NONE        =0,
    GLFW_INIT,
    GLAD_INIT,
	CONTEX_INIT,
	WINDOW_INIT,
	TIMING_INIT,
    SHAPES_INIT,
    TEXT_INIT
};

extern std::unique_ptr<Timing::Timings> timings;

Errors				init();
Errors 				end();

extern std::unique_ptr<Video::VideoOutputBase> videoOutputFromFile(const std::string& file);

}
