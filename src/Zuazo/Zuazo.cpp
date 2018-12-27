#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Zuazo.h"
#include "Context.h"
#include "Window.h"

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
    err=Context::init();
    if(err)
    	return Error::CONTEX_INIT;
    Context::mainCtx->setActive();

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
    return Error::NONE; //TODO
}

}
