#include "Context.h"

#include <stddef.h>
#include <deque>
#include <iterator>

using namespace Zuazo::Graphics;

//Holds the main context, which gets instanceated by init()
std::unique_ptr<const Context> Context::s_mainCtx;

//Holds the main context's glfw context
GLFWwindow* Context::s_mainGlfwCtx=nullptr;

thread_local std::stack<const Context*> Context::s_activeContext( std::deque<const Context*>{nullptr} );

//Stores all the available unique contexts
std::vector<std::unique_ptr<Context>> SharedContext::s_sharedContexts;


/*
 * @brief initializes the context environment
 * @return generated error
 */
int Context::init() {
	s_mainGlfwCtx=nullptr;

	//Create the main context
	s_mainCtx=std::unique_ptr<const Context>(new Context);

	//Set the reference for the main GLFW context
	s_mainGlfwCtx=s_mainCtx->m_glfwCtx;

	//All should be OK
	return 0;
}

/*
 * @brief terminates the context environment
 * @return generated error
 */
int Context::end() {
	s_mainCtx.reset();
	s_mainGlfwCtx=nullptr;
	return 0;
}

Context::Context() {
	//Create a window-less context for me. It will be sharing
	// objects with s_defaultCtx
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    m_glfwCtx=glfwCreateWindow(640, 480, "", NULL, s_mainGlfwCtx);
    glfwDefaultWindowHints();

    glEnable(GL_TEXTURE_2D);
}

Context::~Context() {
	glfwDestroyWindow(m_glfwCtx);
}

const Context& SharedContext::lockAvalibleContext(){
	auto ite=s_sharedContexts.begin();

	if(ite == s_sharedContexts.end()){
		//There are no available contexts
		s_sharedContexts.emplace_back();
		ite=s_sharedContexts.begin();
	}

	while((*ite)->tryUse()){
		++ite;
		if(ite == s_sharedContexts.end()){
			if(s_sharedContexts.size() < MAX_SHARED_CONTEXTS){
				//All contexts are in use, but a new one can be created
				s_sharedContexts.emplace_back();
				ite=s_sharedContexts.end() - 1;
			}else{
				//All contexts are in use, and there is no room for another. Start over
				ite=s_sharedContexts.begin();
			}
		} // else: all should be fine :-)
	}

	return **ite;
}
