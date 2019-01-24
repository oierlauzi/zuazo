#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sys/types.h>
#include <mutex>
#include <string>
#include <vector>

namespace Zuazo{
	class Window;
}

namespace Zuazo::Graphics::GL{

/*
 *Context class
 */
class Context{
	friend Window;
public:
	static int init();
	static int end();

	Context();
	Context(const Context& ctx)=delete;
	~Context();

	void 						setActive() const;
	void 						setPrevActive() const;
	void						use()const;
	bool						tryUse() const;
	void 						unuse() const;

	static const Context* 		mainCtx;
private:
	GLFWwindow *				m_glfwCtx;
	mutable GLFWwindow *		m_prevGlfwCtx;
	mutable std::mutex			m_mutex;

	static GLFWwindow *			s_mainGlfwCtx;
};

/*
 * Locks the given context when instanceated and unlocks it when destroyed
 */
class UniqueContext{
public:
	UniqueContext();
	UniqueContext(const Context& ctx);
	UniqueContext(const Context* ctx);
	UniqueContext(const UniqueContext& ctx)=delete;
	~UniqueContext();
private:
	static const u_int32_t 		MAX_SHARED_CONTEXTS=32;

	const Context* m_ctx;

	static std::vector<Context*> s_sharedContexts;
};


/*
 * INLINE FUNCTION DECLARATIONS
 */

/*
 * @brief sets this context as active
 */
inline void	Context::setActive() const{
	m_prevGlfwCtx=glfwGetCurrentContext(); //Query the previous context

	if(m_glfwCtx!=m_prevGlfwCtx)
		glfwMakeContextCurrent(m_glfwCtx); //This context is not active. Set it as active
}

/*
 * @brief Sets the previous context as active
 */
inline void	Context::setPrevActive() const{
	if(m_glfwCtx!=m_prevGlfwCtx)
		glfwMakeContextCurrent(m_prevGlfwCtx); //The context has hanged. Set the previous active
}

/*
 * @brief Sets this context as active and locks it's mutex
 */
inline void	Context::use()const{
	m_mutex.lock(); //Wait until it can be used in this thread
	setActive(); //Set this context as the current
}

/*
 * @brief If available, sets this context as current
 * @return True if success, false if it is in use elsewhere
 */
inline bool	Context::tryUse() const{
	if(m_mutex.try_lock()){ //Try to lock the mutex
		setActive(); //Set this context as the current
		return true;
	}else return false;
}

/*
 * @brief unlocks context's mutex and restores the previous context
 */
inline void Context::unuse() const{
	setPrevActive();
	m_mutex.unlock(); //Enable others to access this thread
}






inline UniqueContext::UniqueContext(){
	//Find a context
	u_int32_t i=0;
	do{
		if(s_sharedContexts.size()<=i && i< MAX_SHARED_CONTEXTS){
			//All the available contexts are in use but we can still create a new context
			s_sharedContexts.emplace_back(new Context());
		}

		m_ctx=s_sharedContexts[i];

		i=(i + 1) % MAX_SHARED_CONTEXTS;
	}while(!m_ctx->tryUse());
}

inline UniqueContext::UniqueContext(const Context& ctx) : UniqueContext(&ctx){

}

inline UniqueContext::UniqueContext(const Context* ctx){
	m_ctx=ctx;
	m_ctx->use();
}

inline UniqueContext::~UniqueContext(){
	m_ctx->unuse();
}

}
