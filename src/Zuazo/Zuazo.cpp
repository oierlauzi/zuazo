#include "Zuazo.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Graphics/GL/Context.h"
#include "Window.h"
#include "Timing/Timing.h"

namespace Zuazo{

/***************************************************
 *  INITIALIZATION / QUIT FUNCTIONS                *
 ***************************************************/

/**
	@brief initializes Zuazo
	@return Error generated initializing Error::NONE for all OK
 **/
Error init(){
    int err;

    //TODO: logging

    //Init GLFW
    err=glfwInit();
    if(err!=GLFW_TRUE)
        return Error::GLFW_INIT;

    //Initialize context handling
    err=Graphics::GL::Context::init();
    if(err)
    	return Error::CONTEX_INIT;

    Graphics::GL::UniqueContext ctx(Graphics::GL::Context::mainCtx);

    //Init GLEW
    err=glewInit();
    if(err!=GLEW_OK)
        return Error::GLEW_INIT;

    //Enable all needed features
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    //Initialize window class
    err=Window::init();
    if(err)
    	return Error::WINDOW_INIT;

    //Initialize timing
    err=Timing::init();
    if(err)
    	return Error::TIMING_INIT;

//TODO
    /*err=Shapes::Base::init();
    if(err!=OK)
        return Error::SHAPES_INIT;


    err=Text::init();
    if(err!=OK)
        return Error::TEXT_INIT;*/

    return Error::NONE;
}

/**
	@brief ends Zuazo. No more zuazo functions can be called
	@return Error generated destroying Error::NONE for all OK
 **/
Error end(){
	Window::end();
	Timing::end();
    return Error::NONE; //TODO
}

}
