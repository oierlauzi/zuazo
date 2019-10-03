#pragma once

#include "Resolution.h"
#include "Monitor.h"
#include "../Timing/Chrono.h"

#include <memory>
#include <string>

#define GLFW_INCLUDE_NONE //Don't include GL
#include <GLFW/glfw3.h>

namespace Zuazo::Timing {
    class MainThread;
}

namespace Zuazo::Graphics {

class Window {
    friend Timing::MainThread;
public:
    enum class State {
        NORMAL,
        ICONIFIED,
        MAXIMIZED,
        FULLSCREEN,
        HIDDEN
    };

    Window();
    Window(const Resolution& resolution, const std::string& name);
    Window(const Window& other);
    virtual ~Window();

    void            bind();
    void            unbind();

    void            setName(std::string&& name);
    const std::string& getName() const;

    void            setState(State st);
    State           getState() const;

    void            setMonitor(const Monitor& mon = Monitor::getMainMonitor());
    Monitor         getMonitor() const;

    void            setPosition(const Math::Vec2d& pos);
    Math::Vec2d     getPosition() const;

    void            setResolution(const Resolution& res);
    Resolution      getResolution() const;

    void            setRate(const Timing::Rate& rate);
    Timing::Rate    getRate() const;

    void            setVSync(bool vsync);
    bool            getVSync() const;

    void            swapBuffers() const;

    static Window&  getMainWindow();
private:
    GLFWwindow*     m_window;

    std::string     m_name;
    Monitor         m_monitor;

    static std::unique_ptr<Window> s_mainWindow;
    static void     setup();
    static void     cleanup();
};

}