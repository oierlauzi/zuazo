#include "Context.h"

#include <GLFW/glfw3.h>



using namespace Zuazo;

/*
 *	CONTEXT
 */

//Holds the main context, which gets instanceated by init()
std::unique_ptr<Context> 	Context::mainCtx;

//Holds the main context's glfw context
GLFWwindow * Context::s_mainGlfwCtx=NULL;


/*
 * @brief initializes the context environment
 * @return generated error
 */
int Context::init() {
	//Create the main context
	mainCtx=std::unique_ptr<Context>(new Context());

	//Set the reference for the main GLFW context
	s_mainGlfwCtx=mainCtx->s_mainGlfwCtx;

	//All should be OK
	return 0;
}

/*
 * @brief terminates the context environment
 * @return generated error
 */
int Context::end() {
	return 0;
}

Context::Context() {
	//Create a window-less context for me. It will be sharing
	// objects with s_defaultCtx
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    m_glfwCtx=glfwCreateWindow(640, 480, "", NULL, s_mainGlfwCtx);
    glfwDefaultWindowHints();
}

Context::~Context() {
	glfwDestroyWindow(m_glfwCtx);
}

/*
 * @brief sets this context as active
 */
inline void Context::setActive() const{
	Context * ncCtx=const_cast<Context*>(this);

	if(m_glfwCtx!=glfwGetCurrentContext())
		glfwMakeContextCurrent(ncCtx->m_glfwCtx); //This context is not active. Set it as active
}

/*
 * @brief sets this context as active and locks it's mutex
 */
inline void Context::use() const{
	Context * ncCtx=const_cast<Context*>(this);

	ncCtx->m_mutex.lock(); //Wait until it can be used in this thread
	setActive(); //Set this context as the current
}

/*
 * @brief unlocks it's mutex
 */
inline void Context::unuse() const{
	Context * ncCtx=const_cast<Context*>(this);

	ncCtx->m_mutex.unlock(); //Enable others to access this thread
}

/*
 * 	UNIQUE CONTEXT
 */

UniqueContext::UniqueContext(const Context& ctx) {
	m_ctx=&ctx;
	m_ctx->use();
}

UniqueContext::~UniqueContext() {
	m_ctx->unuse();
}

/*
 * 	DEFAULT CONTEXT
 */

//A context pointer for each thread. In case it's needed
//by setDefaultActive(), it will be constructed, otherwise it will be an empty pointer
thread_local std::unique_ptr<Context> threadCtx;

/*
 * @brief sets as active the context bound this thread
 */
inline void setDefaultContextActive(){
	if(threadCtx)
		threadCtx->setActive(); //Context exists, set it active
	else{
		threadCtx=std::unique_ptr<Context>(new Context()); //Does not exist -> create it
		threadCtx->setActive();	//Set it active
	}
}
