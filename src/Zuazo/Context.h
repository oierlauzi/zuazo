#pragma once

#include <sys/types.h>
#include <memory>

#include "../../include/Window.h"
#include "../../include/Zuazo.h"

struct GLFWwindow;

namespace Zuazo{

class Window;

class Context{
	friend Error Zuazo::init();
	friend Window::Window(u_int32_t width, u_int32_t height, std::string name);
public:
	static int init();
	static int end();
	Context();
	Context(const Context& ctx)=delete;
	~Context();

	void						setActive();
	static void 				setDefaultActive();
	static void					setMainActive();
private:
	GLFWwindow *				m_ctx; //The GLFW context I represent

	static thread_local std::unique_ptr<Context> s_threadCtx;
	static GLFWwindow *			s_defaultGLFWCtx;
};

}
