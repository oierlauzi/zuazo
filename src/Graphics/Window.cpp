#include <Graphics/Window.h>

namespace Zuazo::Graphics {

#define ET_EXEC(x, ...)                 \
    s_eventThread->execute(             \
        std::function{x}, __VA_ARGS__   \
    )

/*
 * MONITOR
 */

Window::Monitor::Monitor(GLFWmonitor* mon) :
    m_monitor(mon)
{
}

Window::Monitor::Monitor(Monitor&& mon) :
    m_monitor(mon.m_monitor)
{
    mon.m_monitor = nullptr;
}

Window::Monitor& Window::Monitor::operator=(Monitor&& other) {
    m_monitor = other.m_monitor;
    other.m_monitor = nullptr;

    return *this;
}




bool operator==(const Window::Monitor& x, const Window::Monitor& y){
    return x.m_monitor == y.m_monitor;
}

Window::Monitor::operator bool() const{
    return m_monitor;
}




std::string Window::Monitor::getName() const{
    const char* name = ET_EXEC(glfwGetMonitorName, m_monitor);
    return std::string(name);
}

Math::Vec2i Window::Monitor::getPosition() const{
    Math::Vec2i result;
    ET_EXEC(glfwGetMonitorPos, m_monitor, &result.x, &result.y);
    return result;
}

Math::Vec2d Window::Monitor::getPhysicalSize() const{
    Math::Vec2i result;
    ET_EXEC(glfwGetMonitorPhysicalSize, m_monitor, &result.x, &result.y);
    return Math::Vec2d(result) / 1e3;
}


Window::Monitor::Mode Window::Monitor::getMode() const{
    const GLFWvidmode* mod = ET_EXEC(glfwGetVideoMode, m_monitor);

    return Mode{
        .colorDepth = ColorDepth(mod->redBits, mod->greenBits, mod->blueBits),
        .size = Math::Vec2i(mod->width, mod->height),
        .frameRate = Timing::Rate(mod->refreshRate, 1)
    };
}

std::vector<Window::Monitor::Mode> Window::Monitor::getModes() const{
    int cnt;
    const GLFWvidmode* mod = ET_EXEC(glfwGetVideoModes, m_monitor, &cnt);

    std::vector<Mode> modes; modes.reserve(cnt);
    for(int i = 0; i < cnt; i++){
        modes.emplace_back(Mode{
            .colorDepth = ColorDepth(mod[i].redBits, mod[i].greenBits, mod[i].blueBits),
            .size = Math::Vec2i(mod[i].width, mod[i].height),
            .frameRate = Timing::Rate(mod[i].refreshRate, 1)
        });
    }

    return modes;
}

Window::Monitor::ColorDepth Window::Monitor::getColorDepth() const{
    return getMode().colorDepth;
}

Math::Vec2i Window::Monitor::getSize() const{
    return getMode().size;
}

Timing::Rate Window::Monitor::getRate() const{
    return getMode().frameRate;
}

/*
 * WINDOW
 */

std::unique_ptr<Window::EventThread> Window::s_eventThread;
std::vector<Window::Monitor> Window::s_monitors;
const Window::Monitor Window::NO_MONITOR;




Window::Window(const Math::Vec2i& size, std::string&& name, const Monitor& mon) :
    m_name(std::forward<std::string>(name)),
    m_window(ET_EXEC(glfwCreateWindow, 
        size.x,
        size.y,
        name.c_str(),
        mon.m_monitor, 
        static_cast<GLFWwindow*>(nullptr)
    ))
{
}

Window::Window(Window&& other) :
    m_name(std::move(other.m_name)),
    m_window(std::move(other.m_window)),
    m_windowedState(std::move(other.m_windowedState))
{
    other.m_window = nullptr;
}

Window::~Window(){
    if(m_window)
        ET_EXEC(glfwDestroyWindow, m_window);
}

Window& Window::operator=(Window&& other) {
    m_name = std::move(other.m_name);
    m_window = std::move(other.m_window);
    m_windowedState = std::move(other.m_windowedState);

    other.m_window = nullptr;

    return *this;
}




bool operator==(const Window& x, const Window& y){
    return x.m_window == y.m_window;
}

Window::operator bool() const {
    return m_window;
}



void Window::bind(){
    glfwMakeContextCurrent(m_window);
}

void Window::unbind(){
    glfwMakeContextCurrent(nullptr);
}


void Window::setName(std::string&& name){
    m_name = std::forward<std::string>(name);
    ET_EXEC(glfwSetWindowTitle, m_window, m_name.c_str());
}

const std::string& Window::getName() const{
    return m_name;
}


void Window::setState(State st){
    auto lastState = getState(); //TODO
    if(lastState != st){
        //Leave it on normal state
        switch (lastState) {
        case State::HIDDEN:
            ET_EXEC(glfwShowWindow, m_window);
            break;

        case State::ICONIFIED:
            ET_EXEC(glfwRestoreWindow, m_window);
            break;

        case State::MAXIMIZED:
            ET_EXEC(glfwRestoreWindow, m_window);
            break;

        case State::FULLSCREEN:
            setMonitor(NO_MONITOR);
            break;
        
        default: //State::NORMAL
            break;
        }

        //Switch to the desired state
        switch (st) {
        case State::HIDDEN:
            ET_EXEC(glfwHideWindow, m_window);
            break;

        case State::ICONIFIED:
            ET_EXEC(glfwIconifyWindow, m_window);
            break;

        case State::MAXIMIZED:
            ET_EXEC(glfwMaximizeWindow, m_window);
            break;

        case State::FULLSCREEN:
            setMonitor(s_monitors[0]);
            break;
        
        default: //State::NORMAL
            break;
        }
    }
}

Window::State Window::getState() const{
    State result;

    if(!ET_EXEC(glfwGetWindowAttrib, m_window, GLFW_VISIBLE)){
        result = State::HIDDEN;
    }else if(ET_EXEC(glfwGetWindowAttrib, m_window, GLFW_ICONIFIED)){
        result = State::ICONIFIED;
    } else if(ET_EXEC(glfwGetWindowAttrib, m_window, GLFW_MAXIMIZED)){
        result = State::MAXIMIZED;
    } else if(ET_EXEC(glfwGetWindowMonitor, m_window)){
        result = State::FULLSCREEN;
    } else {
        result = State::NORMAL;
    }

    return result;
}


void Window::setMonitor(const Monitor& mon){
    if(mon){
        setMonitor(mon, mon.getMode());
    }else {
        setMonitor(NO_MONITOR, Monitor::Mode());
    }
}

void Window::setMonitor(const Monitor& mon, const Monitor::Mode& mod){
    const Monitor& prevMon = getMonitor();

    if(mon != prevMon){
        //State has changed. Evaluate what to do
        if(mon) {
            //It is going to be full-screen
            if(!prevMon){
                //It was windowed. Save its state
                m_windowedState = {
                    .pos = getPosition(),
                    .size = getSize()
                };
            }

            Math::Vec2i pos = mon.getPosition();

            ET_EXEC(glfwSetWindowMonitor, 
                m_window, 
                mon.m_monitor, 
                pos.x,
                pos.y,
                mod.size.x,
                mod.size.y,
                static_cast<int>(mod.frameRate)
            );
        } else {
            //It is going to be windowed
            ET_EXEC(glfwSetWindowMonitor, 
                m_window, 
                static_cast<GLFWmonitor*>(nullptr), 
                m_windowedState->pos.x,
                m_windowedState->pos.y,
                m_windowedState->size.x,
                m_windowedState->size.y,
                0
            );
            m_windowedState = {};
        }
    }
}

const Window::Monitor& Window::getMonitor() const{
    GLFWmonitor* mon = ET_EXEC(glfwGetWindowMonitor, m_window);

    if(mon){
        //Try to find it on the monitor vector
        for(auto ite = s_monitors.cbegin(); ite != s_monitors.cend(); ++ite){
            if(ite->m_monitor == mon){
                return *ite;
            }
        }

        return NO_MONITOR; //Not found. Unexpected
    } else {
        return NO_MONITOR;
    }
}


void Window::setPosition(const Math::Vec2i& pos){
    ET_EXEC(glfwSetWindowPos, m_window, pos.x, pos.y);
}

Math::Vec2i Window::getPosition() const{
    Math::Vec2i result;
    ET_EXEC(glfwGetWindowPos, m_window, &result.x, &result.y);
    return result;
}


void Window::setSize(const Math::Vec2i& size){
    ET_EXEC(glfwSetWindowSize, m_window, size.x, size.y);
}

Math::Vec2i Window::getSize() const{
    Math::Vec2i result;
    ET_EXEC(glfwGetWindowSize, m_window, &result.x, &result.y);
    return result;
}

Resolution Window::getResolution() const{
    int x, y;
    ET_EXEC(glfwGetFramebufferSize, m_window, &x, &y);
    return Resolution(x, y);
}

void Window::swapBuffers() const{
    glfwSwapBuffers(m_window); 
}




Window::EventThread& Window::getEventThread(){
    return *s_eventThread;
}

const std::vector<Window::Monitor>& Window::getMonitors(){
    return s_monitors;
}



void Window::init(){
    s_eventThread = std::unique_ptr<EventThread>(new EventThread);
    
    std::lock_guard<std::mutex> lock(s_eventThread->getMutex());

    //Fill the monitor array
    int cnt;
    GLFWmonitor** mons = glfwGetMonitors(&cnt);

    s_monitors.reserve(cnt);

    for(int i = 0; i < cnt; i++){
        s_monitors.push_back(mons[i]);
    }

    
}

void Window::end(){
    s_eventThread.reset();
}



/*
 * EVENT THREAD
 */

Window::EventThread::EventThread() :
    m_exit(false),
    m_thread(&EventThread::threadFunc, this)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_eventsHandled.wait(lock);
}

Window::EventThread::~EventThread() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_exit = true;
        handleEvents();
    }
    m_thread.join();
}

std::mutex& Window::EventThread::getMutex(){
    return m_mutex;
}




template<typename T, typename... Args>
T Window::EventThread::execute(const std::function<T(Args...)>& func, Args... args) {
    std::unique_lock<std::mutex> lock(m_mutex);
    //Create a future object
    std::future<T> futur = std::async(std::launch::deferred, func, args...);

    //Set the execution
    m_execution = [&futur] {
        futur.wait();
    };

    //Wait until it gets completed
    handleEvents();
    m_eventsHandled.wait(lock);

    //Return result
    return futur.get();
}

void Window::EventThread::threadFunc() {
    std::unique_lock<std::mutex> lock(m_mutex);

    //Initialize GLFW
    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_eventsHandled.notify_all(); //Tell the constructor to continue

    while(!m_exit) {
        lock.unlock();
        glfwWaitEvents();
        lock.lock();

        if(m_execution){
            m_execution();
            m_execution = {};
            m_eventsHandled.notify_one();
        }
    }

    //Terminate GLFW
    glfwTerminate();
}

void Window::EventThread::handleEvents(){
    glfwPostEmptyEvent();
}

}