#pragma once

#include <sys/types.h>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "Window.h"

struct GLFWwindow;

namespace Zuazo{

class Window;
class MainContext;

/*
 *Context class
 */
class Context{
	friend Window::Window(u_int32_t width, u_int32_t height, std::string name);
public:
	static int init();
	static int end();

	Context();
	Context(const Context& ctx)=delete;
	~Context();

	void						setActive() const;
	void						setPrevActive() const;
	void						use()const;
	bool						tryUse() const;
	void						unuse() const;

	static Context* 			mainCtx;
private:
	GLFWwindow *				m_glfwCtx;
	GLFWwindow *				m_prevGlfwCtx;
	std::mutex					m_mutex;

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
 * Thread's context handling
 */

extern void setDefaultContextActive();
extern void setDefaultPrevContextActive();

}
