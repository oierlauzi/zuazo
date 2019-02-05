#pragma once

namespace Zuazo{

//s that initialization and terminating can produce
enum class Errors{
    OK			=0,
    NONE        =0,
    GLFW_INIT,
    GLEW_INIT,
	CONTEX_INIT,
	WINDOW_INIT,
	TIMING_INIT,
    SHAPES_INIT,
    TEXT_INIT
};


Errors				init();
Errors 				end();

}
