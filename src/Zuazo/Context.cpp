#include "Context.h"

#include <GLFW/glfw3.h>



using namespace Zuazo;

/*
 *	CONTEXT
 */

//Holds the main context, which gets instanceated by init()
Context* Context::mainCtx;

//Holds the main context's glfw context
GLFWwindow* Context::s_mainGlfwCtx=NULL;


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

/*
 * @brief sets this context as active
 */
inline void Context::setActive() const{
	Context * ncCtx=const_cast<Context*>(this);

	ncCtx->m_prevGlfwCtx=glfwGetCurrentContext(); //Query the previous context

	if(m_glfwCtx!=m_prevGlfwCtx)
		glfwMakeContextCurrent(m_glfwCtx); //This context is not active. Set it as active
}

/*
 * @brief Sets the previous context as active
 */
inline void Context::setPrevActive() const{
	if(m_glfwCtx!=m_prevGlfwCtx)
		glfwMakeContextCurrent(m_prevGlfwCtx); //The context has hanged. Set the previous active
}

/*
 * @brief Sets this context as active and locks it's mutex
 */
inline void Context::use() const{
	Context * ncCtx=const_cast<Context*>(this);

	ncCtx->m_mutex.lock(); //Wait until it can be used in this thread
	setActive(); //Set this context as the current
}

/*
 * @brief If available, sets this context as current
 * @return True if success, false if it is in use elsewhere
 */
inline bool Context::tryUse() const{
	Context * ncCtx=const_cast<Context*>(this);

	if(ncCtx->m_mutex.try_lock()){ //Try to lock the mutex
		setActive(); //Set this context as the current
		return true;
	}else return false;
}

/*
 * @brief unlocks context's mutex
 */
inline void Context::unuse() const{
	Context * ncCtx=const_cast<Context*>(this);

	setPrevActive();
	ncCtx->m_mutex.unlock(); //Enable others to access this thread
}

/*
 * 	UNIQUE CONTEXT
 */

//Stores all the available unique contexts
std::vector<Context*> UniqueContext::s_sharedContexts;

UniqueContext::UniqueContext(){
	//Find a context
	u_int32_t i=0;
	do{
		if(s_sharedContexts.size()>=i && i< MAX_SHARED_CONTEXTS){
			//All the available contexts are in use but we can still create a new context
			s_sharedContexts.emplace_back(new Context);
		}

		m_ctx=s_sharedContexts[i];

		i=(i + 1) % MAX_SHARED_CONTEXTS;
	}while(!m_ctx->tryUse());
}

UniqueContext::UniqueContext(const Context& ctx) : UniqueContext(&ctx){}

UniqueContext::UniqueContext(const Context* ctx) {
	m_ctx=ctx;
	m_ctx->use();
}

UniqueContext::~UniqueContext() {
	m_ctx->unuse();
}

/*
 * 	DEFAULT CONTEXT
 */

//A context pointer for each thread. In case it's needed
//by setDefaultContextActive(), it will be constructed, otherwise it will be an empty pointer
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

/*
 * @brief sets the previous context as active
 */
inline void setDefaultPrevContextActive(){
	threadCtx->setPrevActive(); //Context should exist, set the previous active
}
