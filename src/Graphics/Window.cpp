#include <Graphics/Window.h>

#include <Timing/MainThread.h>

#define MT_EXEC(x, ...)                 \
    Timing::getMainThread().execute(    \
        std::function{x}, __VA_ARGS__   \
    )             


namespace Zuazo::Graphics {

std::unique_ptr<Window> Window::s_mainWindow;

Window::Window(){

}

Window::Window(const Resolution& resolution, const std::string& name){

}

Window::Window(const Window& other){

}

Window::~Window(){

}

void Window::bind(){
    glfwMakeContextCurrent(m_window);
}

void Window::unbind(){
    glfwMakeContextCurrent(nullptr);
}


void Window::setName(std::string&& name){
    m_name = std::forward<std::string>(name);
    MT_EXEC(glfwSetWindowTitle, m_window, m_name.c_str());
}

const std::string& Window::getName() const{
    return m_name;
}


void Window::setState(State st){
    //TODO
}

Window::State Window::getState() const{
    State result;

    if(MT_EXEC(glfwGetWindowAttrib, m_window, GLFW_ICONIFIED)){
        result = State::ICONIFIED;
    } else if(MT_EXEC(glfwGetWindowAttrib, m_window, GLFW_MAXIMIZED)){
        result = State::MAXIMIZED;
    } else if(MT_EXEC(glfwGetWindowMonitor, m_window)){
        result = State::FULLSCREEN;
    } else if(!MT_EXEC(glfwGetWindowAttrib, m_window, GLFW_VISIBLE)){
        result = State::HIDDEN;
    } else {
        result = State::NORMAL;
    }

}


void Window::setMonitor(const Monitor& mon){

}

Monitor Window::getMonitor() const{

}


void Window::setPosition(const Math::Vec2d& pos){

}

Math::Vec2d Window::getPosition() const{

}


void Window::setResolution(const Resolution& res){

}

Resolution Window::getResolution() const{

}


void Window::setRate(const Timing::Rate& rate){

}

Timing::Rate Window::getRate() const{

}

void Window::setVSync(bool vsync){

}

bool Window::getVSync() const{

}


void Window::swapBuffers() const{
    glfwSwapBuffers(m_window); 
}

Window& Window::getMainWindow(){
    return *s_mainWindow;
}

}