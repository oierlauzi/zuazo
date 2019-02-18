#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sys/types.h>
#include <memory>
#include <mutex>
#include <stack>
#include <vector>

namespace Zuazo::Video::Consumers{
	class Window;
}

namespace Zuazo::Graphics{

/*
 *Context class
 */
class Context{
	friend Video::Consumers::Window;
public:
	static int init();
	static int end();

	Context();
	Context(const Context& ctx)=delete;
	~Context();

	void						use()const;
	bool						tryUse() const;
	void 						unuse() const;

	static const Context&		getMainCtx();
private:
	GLFWwindow *				m_glfwCtx;
	mutable std::mutex			m_mutex;

	static GLFWwindow *			s_mainGlfwCtx;
	static std::unique_ptr<const Context> s_mainCtx;
	static thread_local std::stack<const Context*> s_activeContext;
};

/*
 * Locks the given context when instanceated and unlocks it when destroyed
 */
class UniqueContext{
public:
	UniqueContext(const Context& ctx);
	UniqueContext(const UniqueContext& ctx)=delete;
	~UniqueContext();
private:
	const Context& m_ctx;
};

class SharedContext{
public:
	SharedContext();
	SharedContext(const SharedContext& ctx)=delete;
	~SharedContext();
private:
	static const u_int32_t MAX_SHARED_CONTEXTS=32;

	const Context& m_ctx;

	const Context& lockAvalibleContext();
	static std::vector<std::unique_ptr<Context>> s_sharedContexts;
};


/*
 * INLINE FUNCTION DECLARATIONS
 */

/*
 * @brief Sets this context as active and locks it's mutex
 */
inline void	Context::use()const{
	if(s_activeContext.top() != this){
		//This context is not active. Set it as active
		m_mutex.lock();
		glfwMakeContextCurrent(m_glfwCtx);

		if(s_activeContext.top())
			s_activeContext.top()->m_mutex.unlock();
	}
	s_activeContext.push(this);
}

/*
 * @brief If available, sets this context as current
 * @return True if success, false if it is in use elsewhere
 */
inline bool	Context::tryUse() const{
	if(s_activeContext.top() != this){
		//This context is not active. Try to set it active
		if(m_mutex.try_lock()){
			glfwMakeContextCurrent(m_glfwCtx);
			s_activeContext.top()->m_mutex.unlock();
			s_activeContext.push(this);
			return true;
		} else return false; //Could not lock the mutex
	} else {
		s_activeContext.push(this);
		return true; //It was already active
	}
}

/*
 * @brief unlocks context's mutex and restores the previous context
 */
inline void Context::unuse() const{
	s_activeContext.pop();
	if(s_activeContext.top() != this){
		//I am not active
		if(s_activeContext.top()){
			s_activeContext.top()->m_mutex.lock();
			glfwMakeContextCurrent(s_activeContext.top()->m_glfwCtx);
		}else
			glfwMakeContextCurrent(nullptr);

		m_mutex.unlock();
	}
}

inline const Context& Context::getMainCtx(){
	return *s_mainCtx;
}

inline UniqueContext::UniqueContext(const Context& ctx) :
		m_ctx(ctx)
{
	m_ctx.use();
}

inline UniqueContext::~UniqueContext(){
	m_ctx.unuse();
}

inline SharedContext::SharedContext() :
		m_ctx(lockAvalibleContext())
{

}

inline SharedContext::~SharedContext(){
	m_ctx.unuse();
}

}
