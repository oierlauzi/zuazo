#include "Context.h"

using namespace Zuazo::Graphics;

//Holds the main context, which gets instanceated by init()
const Context* Context::mainCtx;

//Holds the main context's glfw context
GLFWwindow* Context::s_mainGlfwCtx=NULL;

//Stores all the available unique contexts
std::vector<Context*> UniqueContext::s_sharedContexts;


/*
 * @brief initializes the context environment
 * @return generated error
 */
int Context::init() {
	//Create the main context
	mainCtx=new Context();

	//Set the reference for the main GLFW context
	s_mainGlfwCtx=mainCtx->m_glfwCtx;

	//All should be OK
	return 0;
}

/*
 * @brief terminates the context environment
 * @return generated error
 */
int Context::end() {
	delete mainCtx;
	return 0;
}

Context::Context() {
	//Create a window-less context for me. It will be sharing
	// objects with s_defaultCtx
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    m_glfwCtx=glfwCreateWindow(640, 480, "", NULL, s_mainGlfwCtx);
    glfwDefaultWindowHints();

    m_prevGlfwCtx=NULL;
}

Context::~Context() {
	glfwDestroyWindow(m_glfwCtx);
}
