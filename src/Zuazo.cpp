#include "Zuazo.h"

Zuazo::Context* defaultCtx=NULL; //Stores the default context. It gets created at init()

using namespace Zuazo;

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

    //Create a windowless CTX for defaultCtx
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    defaultCtx=glfwCreateWindow(640, 480, "", NULL, NULL);
    glfwDefaultWindowHints();

    //Init GLEW
    err=glewInit();
    if(err!=GLEW_OK)
        return Error::GLEW_INIT;

    //Enable all needed features
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

//TODO:
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
    return Error::NONE; //TODO:
}


/***************************************************
 *  CONTEXT RELATED FUNCTIONS                      *
 ***************************************************/

/**
	@brief creates a Zuazo context
	@return The newliy generated context
 **/
Context* createCtx(){
    Context * ctx;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    ctx=glfwCreateWindow(640, 480, "", NULL, defaultCtx);
    glfwDefaultWindowHints();

    return ctx;
}

/**
	@brief destroys the given context
	@param ctx the context that needs to be destroyed
 **/
void destroyCtx(Context* ctx){
    glfwDestroyWindow(ctx);
}

/**
	@brief sets the default context as the active context
 **/
void setDefaultCtxCurrent(){
    glfwMakeContextCurrent(defaultCtx);
}

/**
	@brief sets the given context as active
	@param ctx the context that is going to become active
 **/
void setCtxCurrent(const Context* ctx){
    Context* ncCtx=const_cast<Context*>(ctx);
    glfwMakeContextCurrent(ncCtx);
}
