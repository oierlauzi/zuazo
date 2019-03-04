#include "Zuazo.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Consumers/Window.h"
#include "Graphics/Context.h"
#include "Timing.h"



namespace Zuazo{

std::unique_ptr<Timing> timings;

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if(err!=GLFW_TRUE)
        return Errors::GLFW_INIT;

    //Initialize context handling
    err=Graphics::Context::init();
    if(err)
    	return Errors::CONTEX_INIT;

    Graphics::UniqueContext ctx(Graphics::Context::getMainCtx());

    //Init GLAD
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        return Errors::GLAD_INIT;

    //Initialize window class
    err=Consumers::Window::init();
    if(err)
    	return Errors::WINDOW_INIT;

    //Initialize timing
    timings=std::unique_ptr<Timing>(new Timing);

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
	Consumers::Window::end();
	timings.reset();
	Graphics::Context::end();
    return Errors::NONE; //TODO
}

}
