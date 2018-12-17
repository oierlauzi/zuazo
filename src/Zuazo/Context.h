#pragma once

#include "Zuazo.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

namespace Zuazo{

class Context{
	friend Error Zuazo::init();
public:
	static int init();
	static int end();
	Context();
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
