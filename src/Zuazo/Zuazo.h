#pragma once

#include <memory>

namespace Zuazo {
class Timing;
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

extern std::unique_ptr<Timing> timings;

Errors				init();
Errors 				end();

}
