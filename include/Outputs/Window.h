#pragma once

#include "../ZuazoBase.h"
#include "../Graphics/GLFW.h"

namespace Zuazo::Outputs{

class Window 
	: public ZuazoBase
{
public:

private:
	Graphics::GLFW::Window		m_window;

};

}