#include <Graphics/Window.h>

/*#define MT_EXEC(x, ...)                 \
    Timing::getMainThread().execute(    \
        std::function{x}, __VA_ARGS__   \
    )             


namespace Zuazo::Graphics {

std::unique_ptr<Window> Window::s_mainWindow;

Window::Window() : 
    m_window(MT_EXEC(glfwCreateWindow, 
        640,
        480,
        "",
        static_cast<GLFWmonitor*>(nullptr), 
        s_mainWindow ? s_mainWindow->m_window : static_cast<GLFWwindow*>(nullptr)
    ))
{
}

Window::Window(const Math::Vec2i& size, std::string&& name) :
    m_name(std::forward<std::string>(name)),
    m_window(MT_EXEC(glfwCreateWindow, 
        size.x,
        size.y,
        name.c_str(),
        static_cast<GLFWmonitor*>(nullptr), 
        s_mainWindow ? s_mainWindow->m_window : static_cast<GLFWwindow*>(nullptr)
    )),
{
}

Window::~Window(){
    MT_EXEC(glfwDestroyWindow, m_window);
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
    auto lastState = getState(); //TODO
    if(lastState != st){
        //Leave it on normal state
        switch (lastState) {
        case State::ICONIFIED:
            MT_EXEC(glfwRestoreWindow, m_window);
            break;

        case State::MAXIMIZED:
            MT_EXEC(glfwRestoreWindow, m_window);
            break;

        case State::HIDDEN:
            MT_EXEC(glfwShowWindow, m_window);
            break;

        case State::FULLSCREEN:
            MT_EXEC(glfwSetWindowMonitor, 
                m_window, 
                static_cast<GLFWmonitor*>(nullptr), 
                m_windowedState->pos.x,
                m_windowedState->pos.y,
                m_windowedState->size.x,
                m_windowedState->size.y,
                0
            );
            m_windowedState.reset();
            break;
        
        default: //State::NORMAL
            break;
        }

        //Switch to the desired state
        switch (lastState) {
        case State::ICONIFIED:
            MT_EXEC(glfwIconifyWindow, m_window);
            break;

        case State::MAXIMIZED:
            MT_EXEC(glfwMaximizeWindow, m_window);
            break;

        case State::HIDDEN:
            MT_EXEC(glfwHideWindow, m_window);
            break;

        case State::FULLSCREEN:
            m_windowedState = {
                .pos = getPosition(),
                .size = getSize()
            };

            WindowGeometry monitorGeom{
                .pos = Math::Vec2i(), //TODO
                .size = Math::Vec2i() //TODO
            };

            MT_EXEC(glfwSetWindowMonitor, 
                m_window, 
                m_monitor.m_monitor, 
                monitorGeom.pos.x,
                monitorGeom.pos.y,
                monitorGeom.size.x,
                monitorGeom.size.y,
                0
            );

            break;
        
        default: //State::NORMAL
            break;
        }
    }
}

Window::State Window::getState() const{
    State result;

    if(MT_EXEC(glfwGetWindowAttrib, m_window, GLFW_ICONIFIED)){
        result = State::ICONIFIED;
    } else if(MT_EXEC(glfwGetWindowAttrib, m_window, GLFW_MAXIMIZED)){
        result = State::MAXIMIZED;
    } else if(!MT_EXEC(glfwGetWindowAttrib, m_window, GLFW_VISIBLE)){
        result = State::HIDDEN;
    } else if(MT_EXEC(glfwGetWindowMonitor, m_window)){
        result = State::FULLSCREEN;
    } else {
        result = State::NORMAL;
    }

}


void Window::setMonitor(const Monitor& mon){
    m_monitor = mon;

    if(MT_EXEC(glfwGetWindowMonitor, m_window)) {
        WindowGeometry monitorGeom{
            .pos = Math::Vec2i(), //TODO
            .size = Math::Vec2i() //TODO
        };

        MT_EXEC(glfwSetWindowMonitor, 
            m_window, 
            m_monitor.m_monitor, 
            monitorGeom.pos.x,
            monitorGeom.pos.y,
            monitorGeom.size.x,
            monitorGeom.size.y,
            0
        );
    }
}

Monitor Window::getMonitor() const{
    auto mon = MT_EXEC(glfwGetWindowMonitor, m_window);
    if(mon){
        return Monitor(mon);
    }else{
        return m_monitor;
    }
}


void Window::setPosition(const Math::Vec2i& pos){
    MT_EXEC(glfwSetWindowPos, m_window, pos.x, pos.y);
}

Math::Vec2i Window::getPosition() const{
    Math::Vec2i result;
    MT_EXEC(glfwGetWindowPos, m_window, &result.x, &result.y);
    return result;
}


void Window::setSize(const Math::Vec2i& size){
    MT_EXEC(glfwSetWindowSize, m_window, size.x, size.y);
}

Math::Vec2i Window::getSize() const{
    Math::Vec2i result;
    MT_EXEC(glfwGetWindowSize, m_window, &result.x, &result.y);
    return result;
}

Resolution Window::getResolution() const{
    int x, y;
    MT_EXEC(glfwGetFramebufferSize, m_window, &x, &y);
    return Resolution(x, y);
}

void Window::swapBuffers() const{
    glfwSwapBuffers(m_window); 
}

Window& Window::getMainWindow(){
    return *s_mainWindow;
}


}*/