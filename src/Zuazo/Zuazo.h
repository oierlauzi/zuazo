#pragma once

namespace Zuazo{

//Errors that initialization and terminating can produce
enum Error{
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


Error				init();
Error 				end();

}
