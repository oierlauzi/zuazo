#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <map>
#include <queue>

namespace Zuazo{

//Errors that initialization and other things can produce
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
