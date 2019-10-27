#pragma once

#include "Resolution.h"
#include "../Math/Vector.h"
#include "../Utils/CrossThreadInvocation.h"
#include "../Timing/Chrono.h"
#include "../Zuazo.h"

#include <atomic>
#include <memory>
#include <string>
#include <optional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional> 
#include <future>

#define GLFW_INCLUDE_NONE //Don't include GL
#include <GLFW/glfw3.h>

namespace Zuazo::Graphics {

class Window {
    friend void Zuazo::init();
    friend void Zuazo::end();
public:

    enum class State {
        NORMAL,
        HIDDEN,
        FULLSCREEN,
        ICONIFIED,
        MAXIMIZED
    };

    class Monitor {
        friend Window;
    public:
        using ColorDepth = Math::Vec3i;

        struct Mode {
            ColorDepth colorDepth;
            Math::Vec2i size;
            Timing::Rate frameRate;
        };
 
        Monitor() = default;
        Monitor(Monitor&& other);   
        ~Monitor() = default;

        Monitor&                            operator=(Monitor&& other);

        friend bool                         operator==(const Monitor& x, const Monitor& y);
        operator bool() const;

        std::string                         getName() const;
        Math::Vec2i                         getPosition() const;
        Math::Vec2d                         getPhysicalSize() const;
        Mode                                getMode() const;
        std::vector<Mode>                   getModes() const;
        ColorDepth                          getColorDepth() const;
        Math::Vec2i                         getSize() const;
        Timing::Rate                        getRate() const;
    private:
        Monitor(GLFWmonitor* mon);

        GLFWmonitor*                        m_monitor = nullptr;
    };

    Window() = default;
    Window(const  Math::Vec2i& size, std::string&& name = "", const Monitor& mon = NO_MONITOR);
    Window(const Window& other) = delete;
    Window(Window&& other);
    virtual ~Window();

    Window&                             operator=(Window&& other);

    friend bool                         operator==(const Window& x, const Window& y);
    operator bool() const;

    void                                bind();
    void                                unbind();

    void                                setName(std::string&& name);
    const std::string&                  getName() const;

    void                                setState(State st);
    State                               getState() const;

    void                                setMonitor(const Monitor& mon);
    void                                setMonitor(const Monitor& mon, const Monitor::Mode& mode);
    const Monitor&                      getMonitor() const;

    void                                setPosition(const Math::Vec2i& pos);
    Math::Vec2i                         getPosition() const;

    void                                setSize(const Math::Vec2i& res);
    Math::Vec2i                         getSize() const;

    Resolution                          getResolution() const;

    void                                swapBuffers() const;

    static const std::vector<Monitor>&  getMonitors();

    static const Monitor NO_MONITOR;
private:
    struct WindowGeometry {
        Math::Vec2i pos, size;
    };

    std::string                         m_name;

    GLFWwindow*                         m_window = nullptr;

    std::optional<WindowGeometry>       m_windowedState;               




    static std::vector<Monitor>         s_monitors;

    static std::atomic<bool>            s_exit;

    static std::thread                  s_mainThread;
    static Utils::CrossThreadInvocation s_mainThreadExecutions;
    static void                         mainThreadFunc();
    template<typename T, typename... Args>
    static T                            mainThreadExecute(const std::function<T(Args...)>& func, Args... args);

    static std::thread                  s_cbkThread;
    static Utils::CrossThreadInvocation s_cbks;
    static void                         cbkThreadFunc();
    static void                         monitorCbk(GLFWmonitor* mon, int evnt);

    static void                         addMonitor(GLFWmonitor* mon);
    static void                         eraseMonitor(GLFWmonitor* mon);
    static const Monitor&               getMonitor(GLFWmonitor* mon);


    static void                         init();
    static void                         end();
};

}