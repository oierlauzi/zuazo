#pragma once

namespace Zuazo{

//Errors that initialization and terminating can produce
enum Error{
    OK			=0,
    NONE        =0,
    GLFW_INIT	=-1,
    GLEW_INIT,
	CONTEX_INIT,
    SHAPES_INIT,
    TEXT_INIT
};


Error				init();
Error 				end();

}
