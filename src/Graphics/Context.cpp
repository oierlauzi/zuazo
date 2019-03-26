#include <Graphics/Context.h>
#include <stddef.h>
#include <deque>

using namespace Zuazo::Graphics;

//Holds the main context, which gets instanceated by init()
std::unique_ptr<const Context> Context::s_mainCtx;

//Holds the main context's glfw context
GLFWwindow* Context::s_mainGlfwCtx=nullptr;

thread_local std::stack<const Context*> Context::s_activeContext( std::deque<const Context*>{nullptr} );

//Stores all the available shared contexts
std::array<std::unique_ptr<const Context>, Context::SHARED_CONTEXTS> Context::s_sharedCtxs;

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

	//Create all the shared contexts
	for(auto ite=s_sharedCtxs.begin(); ite != s_sharedCtxs.end(); ++ite){
		*ite=std::unique_ptr<const Context>(new Context);
	}

	//All should be OK
	return 0;
}

/*
 * @brief terminates the context environment
 * @return generated error
 */
int Context::end() {
	for(auto& sharedCtx : s_sharedCtxs){
		sharedCtx.reset();
	}

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
}

Context::~Context() {
	glfwDestroyWindow(m_glfwCtx);
}

const Context& Context::getAvalibleCtx(){
	auto ite=s_sharedCtxs.begin();

	while((*ite)->tryUse() == false){
		++ite; //Next element

		if(ite == s_sharedCtxs.end()){
			//Start over
			ite=s_sharedCtxs.begin();
		}
	}

	return **ite;
}
