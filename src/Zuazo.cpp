#include <Zuazo.h>

#include <Graphics/Window.h>
#include <Timing/MainLoop.h>

namespace Zuazo {

void init(){
    //Graphics::Window::init();
    Timing::MainLoop::init();
}

void end(){
    Timing::MainLoop::end();
    //Graphics::Window::end();
}

}