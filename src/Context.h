#pragma once

#include "Zuazo.h"
#include <memory>

namespace Zuazo{

class Context{
public:
	static int init();
	static int end();
	Context();
	~Context();

	void						use();
	static void 				setActive();

	static GLFWwindow *			s_defaultGLFWCtx;
private:
	GLFWwindow *				m_ctx; //The GLFW context I represent

	static thread_local std::unique_ptr<Context> s_threadCtx;
};

}
