#pragma once

#include <memory>
#include <string>

namespace Zuazo {
namespace Video {
class VideoSourceBase;
} /* namespace Video */
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

extern Errors		init();
extern Errors 		terminate();

extern std::unique_ptr<Video::VideoSourceBase> videoSourceFromFile(const std::string& file);

}
