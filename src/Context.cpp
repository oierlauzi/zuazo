#include "Context.h"
#include "Zuazo.h"

using namespace Zuazo;

//Holds the default GLFW context, where all GL objects will be shared
//It is initialized by init()
GLFWwindow * Context::s_defaultGLFWCtx=NULL;

//A context pointer for each thread. I case it's demanded
//by setActive(), the actual object will be constructed
thread_local std::unique_ptr<Context> Context::s_threadCtx;


/*
 * @brief initializes the context environment
 * @return generated error
 */
int Context::init() {
    //Creates a window-less GLFW context which will be used to share all objects across
	//other contexts
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    s_defaultGLFWCtx=glfwCreateWindow(640, 480, "", NULL, NULL);
    glfwDefaultWindowHints();

    if(s_defaultGLFWCtx)
    	return 0; //Context was created
    else
    	return -1; //Error creating the context
}

/*
 * @brief terminates the context environment
 * @return generated error
 */
int Context::end() {
	if(s_defaultGLFWCtx){
		//Destroy the default GLFW context
		glfwDestroyWindow(s_defaultGLFWCtx);
		s_defaultGLFWCtx=NULL;
		return 0; //All OK
	}else
		return -1; //init wasn'called/failed
}

Context::Context() {
	//Create a windowless context for me. It will be sharing
	// objects with s_defaultCtx
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    m_ctx=glfwCreateWindow(640, 480, "", NULL, s_defaultGLFWCtx);
    glfwDefaultWindowHints();
}

Context::~Context() {
	glfwDestroyWindow(m_ctx);
}


/*
 * @brief sets this context as active
 */
void Context::use() {
	glfwMakeContextCurrent(m_ctx);
}

/*
 * @brief sets as active the context bound this thread
 */
void Context::setActive() {
	if(s_threadCtx)
		s_threadCtx->use(); //Context exists, set it active
	else{
		s_threadCtx=std::unique_ptr<Context>(new Context()); //Does not exist -> create it
		s_threadCtx->use();	//Set it active
	}

}
