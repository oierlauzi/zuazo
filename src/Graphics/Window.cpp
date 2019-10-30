#include <Graphics/Window.h>

#include <iostream>
#include <cstring>

namespace Zuazo::Graphics {

#define MT_EXEC_NO_ARGS(x)              \
	mainThreadExecute(                  \
		std::function{x}                \
	)

#define MT_EXEC(x, ...)                 \
	mainThreadExecute(                  \
		std::function{x}, __VA_ARGS__   \
	)

#define CT_ASYNC_EXEC_NO_ARGS(x)                            \
	s_cbkThreadExecutions.execute(                          \
		std::function{x}                                    \
	)

#define CT_ASYNC_EXEC(x, ...)                               \
	s_cbkThreadExecutions.execute(                          \
		std::function{x}, __VA_ARGS__                       \
	)

#define CT_EXEC_NO_ARGS(x)                                  \
	s_cbkThreadExecutions.execute(                          \
		Utils::CrossThreadInvocation::waitForCompletion,    \
		std::function{x}                                    \
	)

#define CT_EXEC(x, ...)                                     \
	s_cbkThreadExecutions.execute(                          \
		Utils::CrossThreadInvocation::waitForCompletion,    \
		std::function{x}, __VA_ARGS__                       \

static void theFastestFunctionInTheWorld(){
	return;
}

/*
 * MONITOR
 */

Window::Monitor::Monitor(GLFWmonitor* mon) :
	m_monitor(mon)
{
	//Set the user pointer for callbacks
	glfwSetMonitorUserPointer(m_monitor, this);
}

Window::Monitor::Monitor(Monitor&& mon) :
	m_monitor(mon.m_monitor)
{
	//Set the user pointer for callbacks
	glfwSetMonitorUserPointer(m_monitor, this);

	mon.m_monitor = nullptr;
}

Window::Monitor& Window::Monitor::operator=(Monitor&& other) {
	m_monitor = other.m_monitor;

	//Set the user pointer for callbacks
	glfwSetMonitorUserPointer(m_monitor, this);

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
	const char* name = MT_EXEC(glfwGetMonitorName, m_monitor);
	return std::string(name);
}

Math::Vec2i Window::Monitor::getPosition() const{
	Math::Vec2i result;
	MT_EXEC(glfwGetMonitorPos, m_monitor, &result.x, &result.y);
	return result;
}

Math::Vec2d Window::Monitor::getPhysicalSize() const{
	Math::Vec2i result;
	MT_EXEC(glfwGetMonitorPhysicalSize, m_monitor, &result.x, &result.y);
	return Math::Vec2d(result) / 1e3;
}


Window::Monitor::Mode Window::Monitor::getMode() const{
	const GLFWvidmode* mod = MT_EXEC(glfwGetVideoMode, m_monitor);

	return Mode{
		.colorDepth = ColorDepth(mod->redBits, mod->greenBits, mod->blueBits),
		.size = Math::Vec2i(mod->width, mod->height),
		.frameRate = Timing::Rate(mod->refreshRate, 1)
	};
}

std::vector<Window::Monitor::Mode> Window::Monitor::getModes() const{
	int cnt;
	const GLFWvidmode* mod = MT_EXEC(glfwGetVideoModes, m_monitor, &cnt);

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

const Window::Monitor Window::NO_MONITOR;

std::vector<Window::Monitor> Window::s_monitors;


std::atomic<bool> Window::s_exit;

std::thread Window::s_mainThread;
Utils::CrossThreadInvocation Window::s_mainThreadExecutions;

std::thread Window::s_cbkThread;
std::mutex Window::s_cbkMutex;
Utils::CrossThreadInvocation Window::s_cbkThreadExecutions;




Window::Window(const Math::Vec2i& size, std::string&& name, const Monitor& mon) :
	m_name(std::forward<std::string>(name)),
	m_window(MT_EXEC(glfwCreateWindow, 
		size.x,
		size.y,
		name.c_str(),
		mon.m_monitor, 
		static_cast<GLFWwindow*>(nullptr)
	))
{
	setupWindow();
}

Window::Window(Window&& other) :
	m_name(std::move(other.m_name)),
	m_window(std::move(other.m_window)),
	m_windowedState(std::move(other.m_windowedState)),
	m_callbacks(std::move(other.m_callbacks))
{

	if(m_window){
		setupWindow();
	}

	other.m_window = nullptr;
}

Window::~Window(){
	if(m_window){
		MT_EXEC(glfwDestroyWindow, m_window);
	}
}

Window& Window::operator=(Window&& other) {
	if(m_window){
		MT_EXEC(glfwDestroyWindow, m_window);
	}

	m_name = std::move(other.m_name);
	m_window = std::move(other.m_window);
	m_windowedState = std::move(other.m_windowedState);
	m_callbacks = std::move(other.m_callbacks);

	if(m_window){
		setupWindow();
	}

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
		case State::HIDDEN:
			MT_EXEC(glfwShowWindow, m_window);
			break;

		case State::FULLSCREEN:
			setMonitor(NO_MONITOR);
			break;

		case State::ICONIFIED:
			MT_EXEC(glfwRestoreWindow, m_window);
			break;

		case State::MAXIMIZED:
			MT_EXEC(glfwRestoreWindow, m_window);
			break;
		
		default: //State::NORMAL
			break;
		}

		//Switch to the desired state
		switch (st) {
		case State::HIDDEN:
			MT_EXEC(glfwHideWindow, m_window);
			break;

		case State::FULLSCREEN:
			setMonitor(s_monitors[0]);
			break;

		case State::ICONIFIED:
			MT_EXEC(glfwIconifyWindow, m_window);
			break;

		case State::MAXIMIZED:
			MT_EXEC(glfwMaximizeWindow, m_window);
			break;
		
		default: //State::NORMAL
			break;
		}
	}
}

Window::State Window::getState() const{
	State result;

	if(!MT_EXEC(glfwGetWindowAttrib, m_window, GLFW_VISIBLE)){
		result = State::HIDDEN;
	} else if(MT_EXEC(glfwGetWindowMonitor, m_window)){
		result = State::FULLSCREEN;
	}else if(MT_EXEC(glfwGetWindowAttrib, m_window, GLFW_ICONIFIED)){
		result = State::ICONIFIED;
	} else if(MT_EXEC(glfwGetWindowAttrib, m_window, GLFW_MAXIMIZED)){
		result = State::MAXIMIZED;
	} else {
		result = State::NORMAL;
	}

	return result;
}

void Window::setStateCallback(StateCallback&& cbk){
	std::lock_guard<std::mutex> lock(s_cbkMutex);
	m_callbacks.stateCbk = std::forward<StateCallback>(cbk);
}

const Window::StateCallback& Window::getStateCallback() const{
	return m_callbacks.stateCbk;
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

			MT_EXEC(glfwSetWindowMonitor, 
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
			MT_EXEC(glfwSetWindowMonitor, 
				m_window, 
				static_cast<GLFWmonitor*>(nullptr), 
				m_windowedState->pos.x,
				m_windowedState->pos.y,
				m_windowedState->size.x,
				m_windowedState->size.y,
				0
			);

			if(prevMon){
				//It was not windowed
				m_windowedState = {};
			}
		}
	}
}

const Window::Monitor& Window::getMonitor() const{
	GLFWmonitor* mon = MT_EXEC(glfwGetWindowMonitor, m_window);

	if(mon){
		return getMonitor(mon);
	} else {
		return NO_MONITOR;
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

void Window::setPositionCallback(PositionCallback&& cbk){
	std::lock_guard<std::mutex> lock(s_cbkMutex);
	m_callbacks.positionCbk = std::forward<PositionCallback>(cbk);
}

const Window::PositionCallback& Window::getPositionCallback() const{
	return m_callbacks.positionCbk;
}


void Window::setSize(const Math::Vec2i& size){
	MT_EXEC(glfwSetWindowSize, m_window, size.x, size.y);
}

Math::Vec2i Window::getSize() const{
	Math::Vec2i result;
	MT_EXEC(glfwGetWindowSize, m_window, &result.x, &result.y);
	return result;
}

void Window::setSizeCallback(SizeCallback&& cbk){
	std::lock_guard<std::mutex> lock(s_cbkMutex);
	m_callbacks.sizeCbk = std::forward<SizeCallback>(cbk);
}

const Window::SizeCallback& Window::getSizeCallback() const{
	return m_callbacks.sizeCbk;
}

void Window::setOpacity(float opa){
	MT_EXEC(glfwSetWindowOpacity, m_window, opa);
}

float Window::getOpacity() const{
	return MT_EXEC(glfwGetWindowOpacity, m_window);
}

Resolution Window::getResolution() const{
	int x, y;
	MT_EXEC(glfwGetFramebufferSize, m_window, &x, &y);
	return Resolution(x, y);
}

void Window::setResolutionCallback(ResolutionCallback&& cbk){
	std::lock_guard<std::mutex> lock(s_cbkMutex);
	m_callbacks.resolutionCbk = std::forward<ResolutionCallback>(cbk);
}

const Window::ResolutionCallback& Window::getResolutionCallback() const{
	return m_callbacks.resolutionCbk;
}

Math::Vec2f Window::getScale() const{
	Math::Vec2f result;
	MT_EXEC(glfwGetWindowContentScale, m_window, &result.x, &result.y);
	return result;
}

void Window::setScaleCallback(ScaleCallback&& cbk){
	std::lock_guard<std::mutex> lock(s_cbkMutex);
	m_callbacks.scaleCbk = std::forward<ResolutionCallback>(cbk);
}

const Window::ScaleCallback& Window::getScaleCallback() const{
	return m_callbacks.scaleCbk;
}

void Window::close(){
	MT_EXEC(glfwDestroyWindow, m_window);
	m_window = nullptr;
}

bool Window::shouldClose() const{
	return MT_EXEC(glfwWindowShouldClose, m_window);
}

void Window::setCloseCallback(CloseCallback&& cbk){
	std::lock_guard<std::mutex> lock(s_cbkMutex);
	m_callbacks.closeCbk = std::forward<CloseCallback>(cbk);
}

const Window::CloseCallback& Window::getCloseCallback() const{
	return m_callbacks.closeCbk;
}

void Window::focus(){
	return MT_EXEC(glfwFocusWindow, m_window);
}

void Window::setFocusCallback(FocusCallback&& cbk){
	std::lock_guard<std::mutex> lock(s_cbkMutex);
	m_callbacks.focusCbk = std::forward<FocusCallback>(cbk);
}

const Window::FocusCallback& Window::getFocusCallback() const{
	return m_callbacks.focusCbk;
}

void Window::setRefreshCallback(RefreshCallback&& cbk){
	std::lock_guard<std::mutex> lock(s_cbkMutex);
	m_callbacks.refreshCbk = std::forward<RefreshCallback>(cbk);
}

const Window::RefreshCallback& Window::getRefreshCallback() const{
	return m_callbacks.refreshCbk;
}

void Window::setCallbacks(Callbacks&& cbks){
	m_callbacks = std::forward<Callbacks>(cbks);
}

const Window::Callbacks& Window::getCallbacks() const{
	return m_callbacks;
}

void Window::swapBuffers() const{
	glfwSwapBuffers(m_window); 
}




void Window::setupWindow(){
	glfwSetWindowUserPointer(m_window, this);
	MT_EXEC(setupCbks, m_window);
}




const std::vector<Window::Monitor>& Window::getMonitors(){
	return s_monitors;
}

std::vector<Vulkan::Extension> Window::getRequiredVulkanExtensions(){
	uint32_t glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<Vulkan::Extension> extensions;
	extensions.reserve(glfwExtensionCount);

	for(size_t i = 0; i < glfwExtensionCount; i++){
		Vulkan::Extension ext;
		strncpy(ext.extensionName, glfwExtensions[i], VK_MAX_EXTENSION_NAME_SIZE);
		ext.specVersion = 0;
		extensions.push_back(ext);
	}

	return extensions;
}

void Window::mainThreadFunc(){
	glfwInit();

	//Fill the monitor vector
	int cnt;
	GLFWmonitor** mons = glfwGetMonitors(&cnt);
	s_monitors.reserve(cnt);
	for(int i = 0; i < cnt; i++){
		addMonitor(mons[i]);
	}

	//Set all callbacks
	glfwSetMonitorCallback(monitorCbk);

	//Main loop
	while(!s_exit){
		glfwWaitEvents();
		s_mainThreadExecutions.handleAllExecutions();
	}

	glfwTerminate();
}

template<typename T, typename... Args>
inline T Window::mainThreadExecute(const std::function<T(Args...)>& func, Args... args){
	auto futur = s_mainThreadExecutions.execute(func, std::forward<Args>(args)...);
	glfwPostEmptyEvent();
	return futur->getValue();
}


void Window::cbkThreadFunc(){
	while(!s_exit){
		s_cbkThreadExecutions.waitAndHandleAll();
	}
}


void Window::setupCbks(GLFWwindow* win){
	glfwSetWindowPosCallback(win, positionCbk);
	glfwSetWindowSizeCallback(win, sizeCbk);
	glfwSetWindowCloseCallback(win, closeCbk);
	glfwSetWindowRefreshCallback(win, refreshCbk);
	glfwSetWindowFocusCallback(win, focusCbk);
	glfwSetWindowIconifyCallback(win, iconifyCbk);
	glfwSetWindowMaximizeCallback(win, maximizeCbk);
	glfwSetFramebufferSizeCallback(win, framebufferCbk);
	glfwSetWindowContentScaleCallback(win, scaleCbk);
}

void Window::monitorCbk(GLFWmonitor* mon, int evnt){
	if (evnt == GLFW_CONNECTED) {
		// The monitor was connected
		Window::addMonitor(mon);
	} else if (evnt == GLFW_DISCONNECTED) {
		// The monitor was disconnected
		Window::eraseMonitor(mon);
	}
}

void Window::positionCbk(GLFWwindow* win, int x, int y){
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

	std::lock_guard<std::mutex> lock(s_cbkMutex);
	if(window->m_callbacks.positionCbk){
		CT_ASYNC_EXEC(window->m_callbacks.positionCbk, Math::Vec2i(x, y));
	}
}

void Window::sizeCbk(GLFWwindow* win, int x, int y){
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

	std::lock_guard<std::mutex> lock(s_cbkMutex);
	if(window->m_callbacks.sizeCbk){
		CT_ASYNC_EXEC(window->m_callbacks.sizeCbk, Math::Vec2i(x, y));
	}
}

void Window::closeCbk(GLFWwindow* win){
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

	std::lock_guard<std::mutex> lock(s_cbkMutex);
	if(window->m_callbacks.closeCbk){
		CT_ASYNC_EXEC_NO_ARGS(window->m_callbacks.closeCbk);
	}
}

void Window::refreshCbk(GLFWwindow* win){
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

	std::lock_guard<std::mutex> lock(s_cbkMutex);
	if(window->m_callbacks.refreshCbk){
		CT_ASYNC_EXEC_NO_ARGS(window->m_callbacks.refreshCbk);
	}
}

void Window::focusCbk(GLFWwindow* win, int x){
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

	std::lock_guard<std::mutex> lock(s_cbkMutex);
	if(window->m_callbacks.focusCbk){
		CT_ASYNC_EXEC(window->m_callbacks.focusCbk, static_cast<bool>(x));
	}
}

void Window::iconifyCbk(GLFWwindow* win, int x){
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

	std::lock_guard<std::mutex> lock(s_cbkMutex);
	if(window->m_callbacks.stateCbk){
		if(x == GLFW_TRUE){
			CT_ASYNC_EXEC(window->m_callbacks.stateCbk, State::ICONIFIED);
		} else {
			CT_ASYNC_EXEC(window->m_callbacks.stateCbk, State::NORMAL);
		}
	}
}

void Window::maximizeCbk(GLFWwindow* win, int x){
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

	std::lock_guard<std::mutex> lock(s_cbkMutex);
	if(window->m_callbacks.stateCbk){
		if(x == GLFW_TRUE){
			CT_ASYNC_EXEC(window->m_callbacks.stateCbk, State::MAXIMIZED);
		} else {
			CT_ASYNC_EXEC(window->m_callbacks.stateCbk, State::NORMAL);
		}
	}
}

void Window::framebufferCbk(GLFWwindow* win, int x, int y){
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

	std::lock_guard<std::mutex> lock(s_cbkMutex);
	if(window->m_callbacks.resolutionCbk){
		CT_ASYNC_EXEC(window->m_callbacks.resolutionCbk, Resolution(x, y));
	}
}

void Window::scaleCbk(GLFWwindow* win, float x, float y){
	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

	std::lock_guard<std::mutex> lock(s_cbkMutex);
	if(window->m_callbacks.scaleCbk){
		CT_ASYNC_EXEC(window->m_callbacks.scaleCbk, Math::Vec2f(x, y));
	}
}


void Window::addMonitor(GLFWmonitor* mon){
	s_monitors.emplace_back(Monitor(mon));
}

void Window::eraseMonitor(GLFWmonitor* mon){
	for(auto ite = s_monitors.begin(); ite != s_monitors.end(); ++ite){
		if(ite->m_monitor == mon){
			ite = s_monitors.erase(ite);
		}
	}
}

const Window::Monitor& Window::getMonitor(GLFWmonitor* mon){
	for(auto ite = s_monitors.begin(); ite != s_monitors.end(); ++ite){
		if(ite->m_monitor == mon){
			return *ite;
		}
	}

	return NO_MONITOR; //Unexpected
}




void Window::init(){
	s_mainThread = std::thread(mainThreadFunc);
	MT_EXEC_NO_ARGS(theFastestFunctionInTheWorld);

	s_cbkThread = std::thread(cbkThreadFunc);
	CT_EXEC_NO_ARGS(theFastestFunctionInTheWorld);
}

void Window::end(){
	s_exit = true;

	CT_EXEC_NO_ARGS(theFastestFunctionInTheWorld);
	s_cbkThread.join();

	glfwPostEmptyEvent();
	s_mainThread.join();

	s_monitors.clear();
}


}