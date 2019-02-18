#include "Zuazo.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Graphics/Context.h"
#include "Video/Consumers/Window.h"
#include "Timing/Timing.h"

namespace Zuazo{

/***************************************************
 *  INITIALIZATION / QUIT FUNCTIONS                *
 ***************************************************/

/**
	@brief initializes Zuazo
	@return Error generated initializing Error::NONE for all OK
 **/
Errors init(){
    int err;

    //TODO: logging

    //Init GLFW
    err=glfwInit();
    if(err!=GLFW_TRUE)
        return Errors::GLFW_INIT;

    //Initialize context handling
    err=Graphics::Context::init();
    if(err)
    	return Errors::CONTEX_INIT;

    Graphics::UniqueContext ctx(Graphics::Context::getMainCtx());

    //Init GLEW
    err=glewInit();
    if(err!=GLEW_OK)
        return Errors::GLEW_INIT;

    //Enable all needed features
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    //Initialize window class
    err=Video::Consumers::Window::init();
    if(err)
    	return Errors::WINDOW_INIT;

    //Initialize timing
    err=Timing::init();
    if(err)
    	return Errors::TIMING_INIT;

//TODO
    /*err=Shapes::Base::init();
    if(err!=OK)
        return Error::SHAPES_INIT;


    err=Text::init();
    if(err!=OK)
        return Error::TEXT_INIT;*/

    return Errors::NONE;
}

/**
	@brief ends Zuazo. No more zuazo functions can be called
	@return Error generated destroying Error::NONE for all OK
 **/
Errors end(){
	Video::Consumers::Window::end();
	Timing::end();
	Graphics::Context::end();
    return Errors::NONE; //TODO
}

}
