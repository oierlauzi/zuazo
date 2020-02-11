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
std::atomic<bool> GLFW::s_enableCbks;

std::vector<GLFW::MonitorHandle> GLFW::s_monitors;

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

std::vector<GLFW::Monitor> GLFW::getMonitors(){
	std::lock_guard<std::mutex> lock(s_threadMutex);

	std::vector<GLFW::Monitor> result(s_monitors.size());

	for(size_t i = 0; i < s_monitors.size(); i++){
		result[i] = Monitor(s_monitors[i]);
	}

	return result;
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

	//Initialize some variables
	s_threadExit.store(false);
	s_windowCount.store(0);
	s_enableCbks.store(false);

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
	MonitorHandle* mons = glfwGetMonitors(&cnt);
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
		if(s_windowCount.load() > 0){
			lock.unlock();
			s_enableCbks.store(true);

			glfwWaitEvents();

			s_enableCbks.store(false);
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
	if(s_windowCount.load() > 0){
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
void GLFW::addMonitor(MonitorHandle mon){
	s_monitors.emplace_back(mon);
}

void GLFW::eraseMonitor(MonitorHandle mon){
	s_monitors.erase(std::find(s_monitors.cbegin(), s_monitors.cend(), mon));
}

bool GLFW::isValid(MonitorHandle mon){
	return std::find(s_monitors.cbegin(), s_monitors.cend(), mon ) != s_monitors.cend();
}



//Callbacks

void GLFW::monitorCbk(MonitorHandle mon, int evnt){
	if(s_enableCbks.load()) {
		std::lock_guard<std::mutex> lock(s_threadMutex);

		if (evnt == GLFW_CONNECTED) {
			// The monitor was connected
			addMonitor(mon);
		} else if (evnt == GLFW_DISCONNECTED) {
			// The monitor was disconnected
			eraseMonitor(mon);
		}
	}
}

void GLFW::positionCbk(WindowHandle win, int x, int y){
	if(s_enableCbks.load()) {
		std::lock_guard<std::mutex> lock(s_threadMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.positionCbk){
			window->m_callbacks.positionCbk(Math::Vec2i(x, y));
		}
	}
}

void GLFW::sizeCbk(WindowHandle win, int x, int y){
	if(s_enableCbks.load()) {
		std::lock_guard<std::mutex> lock(s_threadMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.sizeCbk){
			window->m_callbacks.sizeCbk(Math::Vec2i(x, y));
		}
	}
}

void GLFW::closeCbk(WindowHandle win){
	if(s_enableCbks.load()) {
		std::lock_guard<std::mutex> lock(s_threadMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.closeCbk){
			window->m_callbacks.closeCbk();
		}
	}
}

void GLFW::refreshCbk(WindowHandle win){
	if(s_enableCbks.load()) {
		std::lock_guard<std::mutex> lock(s_threadMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.refreshCbk){
			window->m_callbacks.refreshCbk();
		}
	}
}

void GLFW::focusCbk(WindowHandle win, int x){
	if(s_enableCbks.load()) {
		std::lock_guard<std::mutex> lock(s_threadMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.focusCbk){
			window->m_callbacks.focusCbk(static_cast<bool>(x));
		}
	}
}

void GLFW::iconifyCbk(WindowHandle win, int x){
	if(s_enableCbks.load()) {
		std::lock_guard<std::mutex> lock(s_threadMutex);

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

void GLFW::maximizeCbk(WindowHandle win, int x){
	if(s_enableCbks.load()) {
		std::lock_guard<std::mutex> lock(s_threadMutex);

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

void GLFW::framebufferCbk(WindowHandle win, int x, int y){
	if(s_enableCbks.load()) {
		std::lock_guard<std::mutex> lock(s_threadMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.resolutionCbk){
			window->m_callbacks.resolutionCbk(Resolution(x, y));
		}
	}
}

void GLFW::scaleCbk(WindowHandle win, float x, float y){
	if(s_enableCbks.load()) {
		std::lock_guard<std::mutex> lock(s_threadMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.scaleCbk){
			window->m_callbacks.scaleCbk(Math::Vec2f(x, y));
		}
	}
}

/*
 * GLFW::Monitor
 */

GLFW::Monitor::Monitor(MonitorHandle mon) 
	: m_monitor(mon)
{
}

GLFW::Monitor::operator bool() const {
	return ZUAZO_GLFW_THREAD_EXECUTE(isValid, m_monitor);
}

std::string GLFW::Monitor::getName() const {
	return ZUAZO_GLFW_THREAD_EXECUTE(getNameImpl, m_monitor);
}

Math::Vec2i GLFW::Monitor::getPosition() const {
	return ZUAZO_GLFW_THREAD_EXECUTE(getPositionImpl, m_monitor);
}

Math::Vec2d GLFW::Monitor::getPhysicalSize() const {
	return ZUAZO_GLFW_THREAD_EXECUTE(getPhysicalSizeImpl, m_monitor);
}

GLFW::Monitor::Mode GLFW::Monitor::getMode() const {
	return ZUAZO_GLFW_THREAD_EXECUTE(getModeImpl, m_monitor);
}

std::vector<GLFW::Monitor::Mode> GLFW::Monitor::getModes() const {
	return ZUAZO_GLFW_THREAD_EXECUTE(getModesImpl, m_monitor);
}

std::string GLFW::Monitor::getNameImpl(MonitorHandle mon){
	if(isValid(mon)){
		return std::string(glfwGetMonitorName(mon));
	} else {
		return {};
	}
}

Math::Vec2i GLFW::Monitor::getPositionImpl(MonitorHandle mon){
	if(isValid(mon)){
		Math::Vec2i result;
		glfwGetMonitorPos(mon, &result.x, &result.y);
		return result;
	} else {
		return {};
	}
}

Math::Vec2d GLFW::Monitor::getPhysicalSizeImpl(MonitorHandle mon){
	if(isValid(mon)){
		Math::Vec2i result;
		glfwGetMonitorPhysicalSize(mon, &result.x, &result.y);
		return Math::Vec2d(result) * 1e-3;
	} else {
		return {};
	}
}

GLFW::Monitor::Mode GLFW::Monitor::getModeImpl(MonitorHandle mon){
	if(isValid(mon)){
		const GLFWvidmode* mod = glfwGetVideoMode(mon);

		return Mode{
			ColorDepth(mod->redBits, mod->greenBits, mod->blueBits),
			Math::Vec2i(mod->width, mod->height),
			Timing::Rate(mod->refreshRate, 1)
		};
	} else {
		return {};
	}
}

std::vector<GLFW::Monitor::Mode> GLFW::Monitor::getModesImpl(MonitorHandle mon){
	if(isValid(mon)){
		int cnt;
		const GLFWvidmode* mod = glfwGetVideoModes(mon, &cnt);

		std::vector<Mode> modes; modes.reserve(cnt);
		for(int i = 0; i < cnt; i++){
			modes.emplace_back(Mode{
				ColorDepth(mod[i].redBits, mod[i].greenBits, mod[i].blueBits),
				Math::Vec2i(mod[i].width, mod[i].height),
				Timing::Rate(mod[i].refreshRate, 1)
			});
		}

		return modes;
	} else {
		return {};
	}
}


/*
 * GLFW::Window
 */

GLFW::Window::Window(	const  Math::Vec2i& size, 
						const std::string_view& name )
	: m_window(ZUAZO_GLFW_THREAD_EXECUTE(createWindow,
		size.x, size.y, 
		name.data(),
		static_cast<void*>(this)
	))
{
	if(m_window == nullptr){
		throw Exception("Error creating the GLFW window");
	}
}

GLFW::Window::Window(Window&& other) {
	std::lock_guard<std::mutex> lock(GLFW::s_threadMutex);

	m_window = other.m_window;
	other.m_window = nullptr;

	glfwSetWindowUserPointer(m_window, this);
	m_callbacks = std::move(other.m_callbacks);
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

	std::lock_guard<std::mutex> lock(GLFW::s_threadMutex);

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
	ZUAZO_GLFW_THREAD_EXECUTE(setNameImpl, m_window, name);
}



void GLFW::Window::setState(State st){
	ZUAZO_GLFW_THREAD_EXECUTE(setStateImpl, m_window, &m_windowedState, st);
}

GLFW::Window::State GLFW::Window::getState() const{
	return ZUAZO_GLFW_THREAD_EXECUTE(getStateImpl, m_window);
}

void GLFW::Window::setStateCallback(StateCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_threadMutex);
	m_callbacks.stateCbk = std::move(cbk);
}

const GLFW::Window::StateCallback& GLFW::Window::getStateCallback() const{
	return m_callbacks.stateCbk;
}



void GLFW::Window::setMonitor(const Monitor& mon){
	if(mon){
		setMonitor(mon, mon.getMode());
	}else {
		setMonitor(NO_MONITOR, Monitor::Mode());
	}
}

void GLFW::Window::setMonitor(const Monitor& mon, const Monitor::Mode& mode){
	ZUAZO_GLFW_THREAD_EXECUTE(setMonitorImpl, m_window, &m_windowedState, mon, mode);
}

GLFW::Monitor GLFW::Window::getMonitor() const{
	return Monitor(ZUAZO_GLFW_THREAD_EXECUTE(getMonitorImpl, m_window));
}



void GLFW::Window::setPosition(const Math::Vec2i& pos){
	ZUAZO_GLFW_THREAD_EXECUTE(setPositionImpl, m_window, pos);
}

Math::Vec2i GLFW::Window::getPosition() const{
	return ZUAZO_GLFW_THREAD_EXECUTE(getPositionImpl, m_window);
}

void GLFW::Window::setPositionCallback(PositionCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_threadMutex);
	m_callbacks.positionCbk = std::move(cbk);
}

const GLFW::Window::PositionCallback& GLFW::Window::getPositionCallback() const{
	return m_callbacks.positionCbk;
}



void GLFW::Window::setSize(const Math::Vec2i& size){
	ZUAZO_GLFW_THREAD_EXECUTE(setSizeImpl, m_window, size);
}

Math::Vec2i GLFW::Window::getSize() const{
	return ZUAZO_GLFW_THREAD_EXECUTE(getSizeImpl, m_window);
}

void GLFW::Window::setSizeCallback(SizeCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_threadMutex);
	m_callbacks.sizeCbk = std::move(cbk);
}

const GLFW::Window::SizeCallback& GLFW::Window::getSizeCallback() const{
	return m_callbacks.sizeCbk;
}



void GLFW::Window::setOpacity(float opa){
	ZUAZO_GLFW_THREAD_EXECUTE(setOpacityImpl, m_window, opa);
}

float GLFW::Window::getOpacity() const{
	return ZUAZO_GLFW_THREAD_EXECUTE(getOpacityImpl, m_window);
}



Resolution GLFW::Window::getResolution() const{
	return ZUAZO_GLFW_THREAD_EXECUTE(getResolutionImpl, m_window);
}

void GLFW::Window::setResolutionCallback(ResolutionCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_threadMutex);
	m_callbacks.resolutionCbk = std::move(cbk);
}

const GLFW::Window::ResolutionCallback& GLFW::Window::getResolutionCallback() const{
	return m_callbacks.resolutionCbk;
}



Math::Vec2f GLFW::Window::getScale() const{
	return ZUAZO_GLFW_THREAD_EXECUTE(getScaleImpl, m_window);
}

void GLFW::Window::setScaleCallback(ScaleCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_threadMutex);
	m_callbacks.scaleCbk = std::move(cbk);
}

const GLFW::Window::ScaleCallback& GLFW::Window::getScaleCallback() const{
	return m_callbacks.scaleCbk;
}



void GLFW::Window::close(){
	ZUAZO_GLFW_THREAD_EXECUTE(destroyWindow, m_window);
	m_window = nullptr;
}

bool GLFW::Window::shouldClose() const{
	return glfwWindowShouldClose(m_window);
}

void GLFW::Window::setCloseCallback(CloseCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_threadMutex);
	m_callbacks.closeCbk = std::move(cbk);
}

const GLFW::Window::CloseCallback& GLFW::Window::getCloseCallback() const{
	return m_callbacks.closeCbk;
}



void GLFW::Window::focus(){
	ZUAZO_GLFW_THREAD_EXECUTE(focusImpl, m_window);
}

void GLFW::Window::setFocusCallback(FocusCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_threadMutex);
	m_callbacks.focusCbk = std::move(cbk);
}

const GLFW::Window::FocusCallback& GLFW::Window::getFocusCallback() const{
	return m_callbacks.focusCbk;
}



void GLFW::Window::setRefreshCallback(RefreshCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_threadMutex);
	m_callbacks.refreshCbk = std::move(cbk);
}

const GLFW::Window::RefreshCallback& GLFW::Window::getRefreshCallback() const{
	return m_callbacks.refreshCbk;
}


//Create / Destroy
GLFW::WindowHandle GLFW::Window::createWindow(	int x, int y, 
												const char* name,
												void* usrPtr )
{
	//Set Vulkan compatibility
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Create the window
	WindowHandle win = glfwCreateWindow(
		x, y,
		name,
		static_cast<MonitorHandle>(nullptr),
		static_cast<WindowHandle>(nullptr)
	);

	if(win == nullptr){
		return nullptr;
	}

	//Set all callbacks
	glfwSetWindowUserPointer(win, usrPtr);
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

void GLFW::Window::destroyWindow(WindowHandle window) {
	glfwDestroyWindow(window);
	GLFW::s_windowCount.fetch_sub(1);
}



void GLFW::Window::setNameImpl(WindowHandle win, std::string_view name){
	glfwSetWindowTitle(win, name.data());
}



void GLFW::Window::setStateImpl(	WindowHandle win,  
									WindowGeometry* windowedGeom,
									State st )
{
	const auto lastState = getStateImpl(win);

	if(lastState != st){
		//Leave it on normal state
		switch (lastState) {
		case State::HIDDEN:
			glfwShowWindow(win);
			break;

		case State::FULLSCREEN:
			setMonitorImpl(	
				win, 
				windowedGeom, 
				GLFW::NO_MONITOR, 
				Monitor::Mode()
			);
			break;

		case State::ICONIFIED:
			glfwRestoreWindow(win);
			break;

		case State::MAXIMIZED:
			glfwRestoreWindow(win);
			break;
		
		default: //State::NORMAL
			break;
		}

		//Switch to the desired state
		switch (st) {
		case State::HIDDEN:
			glfwHideWindow(win);
			break;

		case State::FULLSCREEN:
			setMonitorImpl(
				win, 
				windowedGeom, 
				Monitor(s_monitors[0]),
				Monitor::getModeImpl(s_monitors[0])
			);
			break;

		case State::ICONIFIED:
			glfwIconifyWindow(win);
			break;

		case State::MAXIMIZED:
			glfwMaximizeWindow(win);
			break;
		
		default: //State::NORMAL
			break;
		}
	}
}

GLFW::Window::State GLFW::Window::getStateImpl(WindowHandle win){
	State result;

	if(!glfwGetWindowAttrib(win, GLFW_VISIBLE)){
		result = State::HIDDEN;
	} else if(glfwGetWindowMonitor(win)){
		result = State::FULLSCREEN;
	}else if(glfwGetWindowAttrib(win, GLFW_ICONIFIED)){
		result = State::ICONIFIED;
	} else if(glfwGetWindowAttrib(win, GLFW_MAXIMIZED)){
		result = State::MAXIMIZED;
	} else {
		result = State::NORMAL;
	}

	return result;
}



void GLFW::Window::setMonitorImpl(	WindowHandle win, 
									WindowGeometry* windowedGeom,
									Monitor newMon, 
									Monitor::Mode mode )
{
	const auto oldMonHandle = getMonitorImpl(win).m_monitor;
	const auto newMonHandle = newMon.m_monitor;

	if(newMonHandle != oldMonHandle){
		//State has changed. Evaluate what to do
		if(isValid(newMonHandle)) {
			//It is going to be full-screen
			if(!oldMonHandle){
				//It was windowed. Save its state
				*windowedGeom = {
					getPositionImpl(win),
					getSizeImpl(win)
				};
			}

			const Math::Vec2i pos = Monitor::getPositionImpl(newMonHandle);

			glfwSetWindowMonitor(
				win, 
				newMonHandle, 
				pos.x,
				pos.y,
				mode.size.x,
				mode.size.y,
				static_cast<int>(mode.frameRate)
			);
		} else {
			//It is going to be windowed
			glfwSetWindowMonitor(
				win, 
				static_cast<MonitorHandle>(nullptr), 
				windowedGeom->pos.x,
				windowedGeom->pos.y,
				windowedGeom->size.x,
				windowedGeom->size.y,
				0
			);
		}
	}
}

GLFW::Monitor GLFW::Window::getMonitorImpl(WindowHandle win){
	return Monitor(glfwGetWindowMonitor(win));
}



void GLFW::Window::setPositionImpl(WindowHandle win, Math::Vec2i pos){
	glfwSetWindowPos(win, pos.x, pos.y);
}

Math::Vec2i GLFW::Window::getPositionImpl(WindowHandle win){
	Math::Vec2i result;
	glfwGetWindowPos(win, &result.x, &result.y);
	return result;
}



void GLFW::Window::setSizeImpl(WindowHandle win, Math::Vec2i size){
	glfwSetWindowSize(win, size.x, size.y);
}

Math::Vec2i GLFW::Window::getSizeImpl(WindowHandle win){
	Math::Vec2i result;
	glfwGetWindowSize(win, &result.x, &result.y);
	return result;
}



void GLFW::Window::setOpacityImpl(WindowHandle win, float opa){
	glfwSetWindowOpacity(win, opa);
}

float GLFW::Window::getOpacityImpl(WindowHandle win){
	return glfwGetWindowOpacity(win);
}



Resolution GLFW::Window::getResolutionImpl(WindowHandle win){
	int x, y;
	glfwGetFramebufferSize(win, &x, &y);
	return Resolution(x, y);
}



Math::Vec2f GLFW::Window::getScaleImpl(WindowHandle win){
	Math::Vec2f result;
	glfwGetWindowContentScale(win, &result.x, &result.y);
	return result;
}


void GLFW::Window::focusImpl(WindowHandle win){
	glfwFocusWindow(win);
}


}