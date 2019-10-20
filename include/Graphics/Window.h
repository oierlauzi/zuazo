#pragma once

#include "Resolution.h"
#include "../Zuazo.h"

#include <memory>
#include <string>
#include <optional>

#define GLFW_INCLUDE_NONE //Don't include GL
#include <GLFW/glfw3.h>

namespace Zuazo::Timing {
    class MainThread;
}

namespace Zuazo::Graphics {

class Window {
    friend void Zuazo::init();
    friend void Zuazo::end();
public:
    enum class State {
        NORMAL,
        ICONIFIED,
        MAXIMIZED,
        HIDDEN,
        FULLSCREEN
    };

    class Monitor {

    };

    Window();
    Window(const  Math::Vec2i& size = Math::Vec2i(640, 480), std::string&& name = "");
    Window(const Window& other) = delete;
    virtual ~Window();

    void            bind();
    void            unbind();

    void            setName(std::string&& name);
    const std::string& getName() const;

    void            setState(State st);
    State           getState() const;

    void            setMonitor(const Monitor& mon);
    Monitor         getMonitor() const;

    void            setPosition(const Math::Vec2i& pos);
    Math::Vec2i     getPosition() const;

    void            setSize(const Math::Vec2i& res);
    Math::Vec2i     getSize() const;

    Resolution      getResolution() const;

    void            swapBuffers() const;

    static Window&  getMainWindow();
private:
    struct WindowGeometry {
        Math::Vec2i pos, size;
    };

    std::string     m_name;

    GLFWwindow*     m_window;
    GLFWmonitor*    m_monitor;

    std::optional<WindowGeometry> m_windowedState;

    static std::unique_ptr<Window> s_mainWindow;
    static void     init();
    static void     end();
};

}