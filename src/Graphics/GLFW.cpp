#include <Graphics/GLFW.h>

#include <Exception.h>

#include <future>

namespace Zuazo::Graphics {

#define ZUAZO_GLFW_THREAD_EXECUTE(func, ...) GLFW::threadExecute(std::function{func}, __VA_ARGS__)
#define ZUAZO_GLFW_THREAD_EXECUTE0(func) GLFW::threadExecute(std::function{func})


/*
 * GLFW
 */

const GLFW::Monitor GLFW::NO_MONITOR = GLFW::Monitor();

std::atomic<size_t> GLFW::s_instanceCount(0);
std::atomic<size_t>	GLFW::s_windowCount(0);

std::thread	GLFW::s_thread;
std::atomic<bool> GLFW::s_threadExit;
std::mutex GLFW::s_threadMutex;
std::condition_variable	GLFW::s_threadContinueCondition;
std::condition_variable	GLFW::s_threadCompleteCondition;
std::queue<std::function<void(void)>> GLFW::s_threadExecutions;

std::atomic<bool> GLFW::s_callbacksEnabled;

std::vector<GLFW::Monitor> GLFW::s_monitors;

GLFW::GLFW(){
	if(s_instanceCount.fetch_add(1) == 0){
		//First instance. Initialze
		initialize();
	}
}

GLFW::~GLFW(){
	if(s_instanceCount.fetch_sub(1) == 1){
		//Last instance. Teminate
		terminate();
	}
}

const std::vector<GLFW::Monitor>& GLFW::getMonitors(){
	return s_monitors;
}


std::vector<vk::ExtensionProperties> GLFW::getRequiredVulkanExtensions(){
	uint32_t glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<vk::ExtensionProperties> extensions;
	extensions.reserve(glfwExtensionCount);

	for(size_t i = 0; i < glfwExtensionCount; i++){
		extensions.emplace_back();
		strncpy(extensions.back().extensionName, glfwExtensions[i], VK_MAX_EXTENSION_NAME_SIZE);
	}

	return extensions;
}

bool GLFW::getPresentationSupport(	const vk::Instance& instance, 
									const vk::PhysicalDevice& device, 
									uint32_t family )
{
	return glfwGetPhysicalDevicePresentationSupport(
		static_cast<VkInstance>(instance), 
		static_cast<VkPhysicalDevice>(device), 
		family
	);
}

std::vector<uint32_t> GLFW::getPresentationQueueFamilies(	const vk::Instance& instance, 
															const vk::PhysicalDevice& device, 
															uint32_t queueCount )
{
	std::vector<uint32_t> result;

	for(uint32_t i = 0; i < queueCount; i++){
		if(getPresentationSupport(instance, device, i)){
			result.push_back(i);
		}
	}

	return result;
}



//Initialization / Termination
void GLFW::initialize(){
	std::unique_lock<std::mutex> lock(s_threadMutex);

	//Set some variables
	s_threadExit.store(false);
	s_callbacksEnabled.store(true);
	s_windowCount.store(0);

	//Initialize thread and wait
	s_thread = std::thread(threadFunc);
	s_threadCompleteCondition.wait(lock);
}

void GLFW::terminate(){
	std::unique_lock<std::mutex> lock(s_threadMutex);
	//Indicate to termiate
	s_threadExit.store(true);
	threadContinue();
	lock.unlock();

	//Wait for termination
	s_thread.join();
}

//Thread
void GLFW::threadFunc(){
	std::unique_lock<std::mutex> lock(s_threadMutex);

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

	while(s_threadExit.load() == false){
		//Notify waiting threads
		s_threadCompleteCondition.notify_all();

		//Wait until an event arises
		if(s_callbacksEnabled.load() && (s_windowCount.load() > 0)){
			lock.unlock();
			glfwWaitEvents();
			lock.lock();
		} else {
			s_threadContinueCondition.wait(lock);
		}

		//Execute all pending executions
		while(s_threadExecutions.size() > 0){
			s_threadExecutions.front()();
			s_threadExecutions.pop();
		}
	}

	glfwTerminate();
}

void GLFW::threadContinue(){
	if(s_callbacksEnabled.load() && (s_windowCount.load() > 0)){
		glfwPostEmptyEvent();
	} else {
		s_threadContinueCondition.notify_all();
	}
}

template<typename T, typename... Args>
T GLFW::threadExecute(std::function<T(Args...)> func, Args... args){
	if(std::this_thread::get_id() == s_thread.get_id()){
		return func(std::move(args)...); //We are on the main thread. Simply execute it
	}else {
		std::unique_lock<std::mutex> lock(s_threadMutex);

		//Create a future object to pass it to the main thread
		auto futur = std::async(
			std::launch::deferred, 
			std::move(func), 
			std::move(args)...
		);
		s_threadExecutions.emplace([&futur] () -> void { futur.wait(); });
		
		//Wait until execution is complete
		threadContinue();
		s_threadCompleteCondition.wait(lock);

		return futur.get();
	}
}


//Monitors
void GLFW::addMonitor(GLFWmonitor* mon){
	s_monitors.emplace_back(Monitor(mon));
}

void GLFW::eraseMonitor(GLFWmonitor* mon){
	for(auto ite = s_monitors.begin(); ite != s_monitors.end(); ++ite){
		if(ite->m_monitor == mon){
			ite = s_monitors.erase(ite);
		}
	}
}

const GLFW::Monitor& GLFW::getMonitor(GLFWmonitor* mon){
	for(const auto& m : s_monitors){
		if(m.m_monitor == mon){
			return m;
		}
	}

	return NO_MONITOR;
}



//Callbacks
void GLFW::setCallbacksEnabled(bool ena){
	std::lock_guard<std::mutex>  lock(s_threadMutex);
	threadContinue();
	s_callbacksEnabled.store(ena);
}

void GLFW::monitorCbk(GLFWmonitor* mon, int evnt){
	if(s_callbacksEnabled.load()) {
		if (evnt == GLFW_CONNECTED) {
			// The monitor was connected
			addMonitor(mon);
		} else if (evnt == GLFW_DISCONNECTED) {
			// The monitor was disconnected
			eraseMonitor(mon);
		}
	}
}

void GLFW::positionCbk(GLFWwindow* win, int x, int y){
	if(s_callbacksEnabled.load()) {
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.positionCbk){
			window->m_callbacks.positionCbk(Math::Vec2i(x, y));
		}
	}
}

void GLFW::sizeCbk(GLFWwindow* win, int x, int y){
	if(s_callbacksEnabled.load()) {
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.sizeCbk){
			window->m_callbacks.sizeCbk(Math::Vec2i(x, y));
		}
	}
}

void GLFW::closeCbk(GLFWwindow* win){
	if(s_callbacksEnabled.load()) {
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.closeCbk){
			window->m_callbacks.closeCbk();
		}
	}
}

void GLFW::refreshCbk(GLFWwindow* win){
	if(s_callbacksEnabled.load()) {
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.refreshCbk){
			window->m_callbacks.refreshCbk();
		}
	}
}

void GLFW::focusCbk(GLFWwindow* win, int x){
	if(s_callbacksEnabled.load()) {
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.focusCbk){
			window->m_callbacks.focusCbk(static_cast<bool>(x));
		}
	}
}

void GLFW::iconifyCbk(GLFWwindow* win, int x){
	if(s_callbacksEnabled.load()) {
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.stateCbk){
			if(x == GLFW_TRUE){
				window->m_callbacks.stateCbk(Window::State::ICONIFIED);
			} else {
				window->m_callbacks.stateCbk(Window::State::NORMAL);
			}
		}
	}
}

void GLFW::maximizeCbk(GLFWwindow* win, int x){
	if(s_callbacksEnabled.load()) {
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.stateCbk){
			if(x == GLFW_TRUE){
				window->m_callbacks.stateCbk(Window::State::MAXIMIZED);
			} else {
				window->m_callbacks.stateCbk(Window::State::NORMAL);
			}
		}
	}
}

void GLFW::framebufferCbk(GLFWwindow* win, int x, int y){
	if(s_callbacksEnabled.load()) {
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.resolutionCbk){
			window->m_callbacks.resolutionCbk(Resolution(x, y));
		}
	}
}

void GLFW::scaleCbk(GLFWwindow* win, float x, float y){
	if(s_callbacksEnabled.load()) {
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.scaleCbk){
			window->m_callbacks.scaleCbk(Math::Vec2f(x, y));
		}
	}
}

/*
 * GLFW::Monitor
 */

GLFW::Monitor::Monitor(GLFWmonitor* mon) 
	: m_monitor(mon)
{
}

GLFW::Monitor::Monitor(Monitor&& other)
	: m_monitor(other.m_monitor)
{
	other.m_monitor = nullptr;
}

GLFW::Monitor& GLFW::Monitor::operator=(Monitor&& other){
	m_monitor = other.m_monitor;
	other.m_monitor = nullptr;

	return *this;
}


GLFW::Monitor::operator bool() const {
	return m_monitor;
}

std::string GLFW::Monitor::getName() const {
	const char* name = ZUAZO_GLFW_THREAD_EXECUTE(glfwGetMonitorName, m_monitor);
	return std::string(name);
}

Math::Vec2i GLFW::Monitor::getPosition() const {
	Math::Vec2i result;
	ZUAZO_GLFW_THREAD_EXECUTE(glfwGetMonitorPos, m_monitor, &result.x, &result.y);
	return result;
}

Math::Vec2d GLFW::Monitor::getPhysicalSize() const {
	Math::Vec2i result;
	ZUAZO_GLFW_THREAD_EXECUTE(glfwGetMonitorPhysicalSize, m_monitor, &result.x, &result.y);
	return Math::Vec2d(result) * 1e-3;
}

GLFW::Monitor::Mode GLFW::Monitor::getMode() const {
	const GLFWvidmode* mod = ZUAZO_GLFW_THREAD_EXECUTE(glfwGetVideoMode, m_monitor);

	return Mode{
		ColorDepth(mod->redBits, mod->greenBits, mod->blueBits),
		Math::Vec2i(mod->width, mod->height),
		Timing::Rate(mod->refreshRate, 1)
	};
}

std::vector<GLFW::Monitor::Mode> GLFW::Monitor::getModes() const {
	int cnt;
	const GLFWvidmode* mod = ZUAZO_GLFW_THREAD_EXECUTE(glfwGetVideoModes, m_monitor, &cnt);

	std::vector<Mode> modes; modes.reserve(cnt);
	for(int i = 0; i < cnt; i++){
		modes.emplace_back(Mode{
			ColorDepth(mod[i].redBits, mod[i].greenBits, mod[i].blueBits),
			Math::Vec2i(mod[i].width, mod[i].height),
			Timing::Rate(mod[i].refreshRate, 1)
		});
	}

	return modes;
}

/*
 * GLFW::Window
 */

GLFW::Window::Window(	const  Math::Vec2i& size, 
						const std::string_view& name )
	: m_window(ZUAZO_GLFW_THREAD_EXECUTE(createWindow,
		size.x, size.y, 
		name.data()
	))
{
	glfwSetWindowUserPointer(m_window, this);
}

GLFW::Window::Window(Window&& other)
	: m_window(other.m_window)
	, m_callbacks(std::move(other.m_callbacks))
{
	other.m_window = nullptr;

	glfwSetWindowUserPointer(m_window, this);
}

GLFW::Window::~Window(){
	if(m_window){
		ZUAZO_GLFW_THREAD_EXECUTE(destroyWindow, m_window);
	}
}

GLFW::Window& GLFW::Window::operator=(Window&& other){
	if(m_window){
		ZUAZO_GLFW_THREAD_EXECUTE(destroyWindow, m_window);
	}

	m_callbacks = std::move(other.m_callbacks);

	m_window = other.m_window;
	other.m_window = nullptr;

	glfwSetWindowUserPointer(m_window, this);

	return *this;
}

GLFW::Window::operator bool() const {
	return m_window;
}



//Getters / Setters
vk::UniqueSurfaceKHR GLFW::Window::getSurface(const Vulkan& vulkan) const {
	using Deleter = vk::UniqueHandleTraits<vk::SurfaceKHR, vk::DispatchLoaderDynamic>::deleter;

	//Try to create the surface
	VkSurfaceKHR surface;
	VkResult err = glfwCreateWindowSurface(
		vulkan.getInstance(), 
		m_window, 
		nullptr, 
		&surface
	);

	if(err != VK_SUCCESS){
		throw Exception("Error creating Vulkan surface");
	}

	return vk::UniqueSurfaceKHR(
		surface,
		Deleter(vulkan.getInstance(), nullptr, vulkan.getDispatcher())
	);
}	


void GLFW::Window::setName(const std::string_view& name){
	ZUAZO_GLFW_THREAD_EXECUTE(glfwSetWindowTitle, 
		m_window, name.data()
	);
}


void GLFW::Window::setState(State st){
	auto lastState = getState();
	if(lastState != st){
		//Leave it on normal state
		switch (lastState) {
		case State::HIDDEN:
			ZUAZO_GLFW_THREAD_EXECUTE(glfwShowWindow, m_window);
			break;

		case State::FULLSCREEN:
			setMonitor(Monitor());
			break;

		case State::ICONIFIED:
			ZUAZO_GLFW_THREAD_EXECUTE(glfwRestoreWindow, m_window);
			break;

		case State::MAXIMIZED:
			ZUAZO_GLFW_THREAD_EXECUTE(glfwRestoreWindow, m_window);
			break;
		
		default: //State::NORMAL
			break;
		}

		//Switch to the desired state
		switch (st) {
		case State::HIDDEN:
			ZUAZO_GLFW_THREAD_EXECUTE(glfwHideWindow, m_window);
			break;

		case State::FULLSCREEN:
			setMonitor(GLFW::getMonitors()[0]);
			break;

		case State::ICONIFIED:
			ZUAZO_GLFW_THREAD_EXECUTE(glfwIconifyWindow, m_window);
			break;

		case State::MAXIMIZED:
			ZUAZO_GLFW_THREAD_EXECUTE(glfwMaximizeWindow, m_window);
			break;
		
		default: //State::NORMAL
			break;
		}
	}
}

GLFW::Window::State GLFW::Window::getState() const{
	State result;

	if(!ZUAZO_GLFW_THREAD_EXECUTE(glfwGetWindowAttrib, m_window, GLFW_VISIBLE)){
		result = State::HIDDEN;
	} else if(ZUAZO_GLFW_THREAD_EXECUTE(glfwGetWindowMonitor, m_window)){
		result = State::FULLSCREEN;
	}else if(ZUAZO_GLFW_THREAD_EXECUTE(glfwGetWindowAttrib, m_window, GLFW_ICONIFIED)){
		result = State::ICONIFIED;
	} else if(ZUAZO_GLFW_THREAD_EXECUTE(glfwGetWindowAttrib, m_window, GLFW_MAXIMIZED)){
		result = State::MAXIMIZED;
	} else {
		result = State::NORMAL;
	}

	return result;
}

void GLFW::Window::setMonitor(const Monitor& mon){
	if(mon){
		setMonitor(mon, mon.getMode());
	}else {
		setMonitor(NO_MONITOR, Monitor::Mode());
	}
}

void GLFW::Window::setMonitor(const Monitor& mon, const Monitor::Mode& mode){
	const Monitor& prevMon = getMonitor();

	if(mon.m_monitor != prevMon.m_monitor){
		//State has changed. Evaluate what to do
		if(mon) {
			//It is going to be full-screen
			if(!prevMon){
				//It was windowed. Save its state
				m_windowedState = {
					getPosition(),
					getSize()
				};
			}

			Math::Vec2i pos = mon.getPosition();

			ZUAZO_GLFW_THREAD_EXECUTE(glfwSetWindowMonitor, 
				m_window, 
				mon.m_monitor, 
				pos.x,
				pos.y,
				mode.size.x,
				mode.size.y,
				static_cast<int>(mode.frameRate)
			);
		} else {
			//It is going to be windowed
			ZUAZO_GLFW_THREAD_EXECUTE(glfwSetWindowMonitor, 
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

const GLFW::Monitor& GLFW::Window::getMonitor() const{
	GLFWmonitor* mon = ZUAZO_GLFW_THREAD_EXECUTE(glfwGetWindowMonitor, m_window);

	if(mon){
		return GLFW::getMonitor(mon);
	} else {
		return GLFW::NO_MONITOR;
	}
}


void GLFW::Window::setPosition(const Math::Vec2i& pos){
	ZUAZO_GLFW_THREAD_EXECUTE(glfwSetWindowPos, m_window, pos.x, pos.y);
}

Math::Vec2i GLFW::Window::getPosition() const{
	Math::Vec2i result;
	ZUAZO_GLFW_THREAD_EXECUTE(glfwGetWindowPos, m_window, &result.x, &result.y);
	return result;
}


void GLFW::Window::setSize(const Math::Vec2i& size){
	ZUAZO_GLFW_THREAD_EXECUTE(glfwSetWindowSize, m_window, size.x, size.y);
}

Math::Vec2i GLFW::Window::getSize() const{
	Math::Vec2i result;
	ZUAZO_GLFW_THREAD_EXECUTE(glfwGetWindowSize, m_window, &result.x, &result.y);
	return result;
}


void GLFW::Window::setOpacity(float opa){
	ZUAZO_GLFW_THREAD_EXECUTE(glfwSetWindowOpacity, m_window, opa);
}

float GLFW::Window::getOpacity() const{
	return ZUAZO_GLFW_THREAD_EXECUTE(glfwGetWindowOpacity, m_window);
}

Resolution GLFW::Window::getResolution() const{
	int x, y;
	ZUAZO_GLFW_THREAD_EXECUTE(glfwGetFramebufferSize, m_window, &x, &y);
	return Resolution(x, y);
}


Math::Vec2f GLFW::Window::getScale() const{
	Math::Vec2f result;
	ZUAZO_GLFW_THREAD_EXECUTE(glfwGetWindowContentScale, m_window, &result.x, &result.y);
	return result;
}


void GLFW::Window::close(){
	ZUAZO_GLFW_THREAD_EXECUTE(glfwDestroyWindow, m_window);
	m_window = nullptr;
}

bool GLFW::Window::shouldClose() const{
	return ZUAZO_GLFW_THREAD_EXECUTE(glfwWindowShouldClose, m_window);
}


void GLFW::Window::focus(){
	return ZUAZO_GLFW_THREAD_EXECUTE(glfwFocusWindow, m_window);
}


void GLFW::Window::setCallbacks(Callbacks&& cbks){
	m_callbacks = std::move(cbks);
}

const GLFW::Window::Callbacks& GLFW::Window::getCallbacks() const{
	return m_callbacks;
}


//Create / Destroy
GLFWwindow* GLFW::Window::createWindow(	int x, int y, 
										const char* name )
{
	//Set Vulkan compatibility
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Create the window
	GLFWwindow* win = glfwCreateWindow(
		x, y,
		name,
		static_cast<GLFWmonitor*>(nullptr),
		static_cast<GLFWwindow*>(nullptr)
	);

	if(win == nullptr){
		throw Exception("Error creating the window");
	}

	//Set all callbacks
	glfwSetWindowPosCallback(win, GLFW::positionCbk);
	glfwSetWindowSizeCallback(win, GLFW::sizeCbk);
	glfwSetWindowCloseCallback(win, GLFW::closeCbk);
	glfwSetWindowRefreshCallback(win, GLFW::refreshCbk);
	glfwSetWindowFocusCallback(win, GLFW::focusCbk);
	glfwSetWindowIconifyCallback(win, GLFW::iconifyCbk);
	glfwSetWindowMaximizeCallback(win, GLFW::maximizeCbk);
	glfwSetFramebufferSizeCallback(win, GLFW::framebufferCbk);
	glfwSetWindowContentScaleCallback(win, GLFW::scaleCbk);


	GLFW::s_windowCount.fetch_add(1);
	return win;
}

void GLFW::Window::destroyWindow(GLFWwindow* window) {
	glfwDestroyWindow(window);
	GLFW::s_windowCount.fetch_sub(1);
}

}