#pragma once

#include <set>
#include <GLFW/glfw3.h>

namespace Zuazo::Graphics {

class Window;

class Monitor {
    friend Window;
public:
    Monitor() = default;
    Monitor(const Monitor& other) = default;
    ~Monitor() = default;



    static std::set<Monitor>        getMonitors();
    static Monitor                  getMainMonitor();                
private:
    Monitor(GLFWmonitor* monitor);

    GLFWmonitor*                    m_monitor = nullptr;
};

}