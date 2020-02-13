#include <Graphics/GLFW.h>

#include <Exception.h>

#include <future>

namespace Zuazo::Graphics {

/*
 * GLFW
 */

const GLFW::Monitor GLFW::NO_MONITOR = GLFW::Monitor();


std::atomic<size_t> GLFW::s_instanceCount(0);
std::unique_ptr<GLFW::Thread> GLFW::s_thread;
std::mutex GLFW::s_cbkMutex;

GLFW::MonitorCallback GLFW::s_monitorCbk;
GLFW::MonitorHandle GLFW::s_activeMonitorCbk(nullptr);


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
	return s_thread->execute<std::vector<Monitor>>(getMonitorsImpl);
}

void GLFW::setMonitorCallback(MonitorCallback&& cbk){
	std::lock_guard<std::mutex> lock(s_cbkMutex);
	s_monitorCbk = std::move(cbk);
}

const GLFW::MonitorCallback& GLFW::getMonitorCallback(){
	return s_monitorCbk;
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
	s_thread = std::make_unique<Thread>();
}

void GLFW::terminate(){
	s_thread.reset();
}



std::vector<GLFW::Monitor> GLFW::getMonitorsImpl(){

	int cnt;
	MonitorHandle* monitors = glfwGetMonitors(&cnt);

	std::vector<GLFW::Monitor> result(cnt);
	for(size_t i = 0; i < result.size(); i++){
		result[i] = Monitor(monitors[i]);
	}

	return result;
}


void GLFW::monitorCbk(MonitorHandle mon, int evnt){
	if(s_thread->getCallbacksEnabled()) {
		std::lock_guard<std::mutex> lock(s_cbkMutex);

		if(s_monitorCbk){
			s_activeMonitorCbk = mon;

			switch(evnt){
			case GLFW_CONNECTED: 
				s_monitorCbk(Monitor(mon), MonitorEvent::CONNECTED); 
				break;
			case GLFW_DISCONNECTED: 
				s_monitorCbk(Monitor(mon), MonitorEvent::DISCONNECTED); 
				break;
			default:
				break;
			}

			s_activeMonitorCbk = nullptr;
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
	return s_thread->execute<bool>(isValidImpl, m_monitor);
}

std::string GLFW::Monitor::getName() const {
	return s_thread->execute<std::string>(getNameImpl, m_monitor);
}

Math::Vec2i GLFW::Monitor::getPosition() const {
	return s_thread->execute<Math::Vec2i>(getPositionImpl, m_monitor);
}

Math::Vec2d GLFW::Monitor::getPhysicalSize() const {
	return s_thread->execute<Math::Vec2d>(getPhysicalSizeImpl, m_monitor);
}

GLFW::Monitor::Mode GLFW::Monitor::getMode() const {
	return s_thread->execute<Mode>(getModeImpl, m_monitor);
}

std::vector<GLFW::Monitor::Mode> GLFW::Monitor::getModes() const {
	return s_thread->execute<std::vector<Mode>>(getModesImpl, m_monitor);
}



bool GLFW::Monitor::isValidImpl(MonitorHandle mon){
	if(s_activeMonitorCbk == mon){
		return true;
	}

	int cnt;
	MonitorHandle* start = glfwGetMonitors(&cnt);
	MonitorHandle* end = start + cnt;

	return std::find(start, end + cnt, mon) != end;
}

std::string GLFW::Monitor::getNameImpl(MonitorHandle mon){
	if(isValidImpl(mon)){
		return std::string(glfwGetMonitorName(mon));
	} else {
		return {};
	}
}

Math::Vec2i GLFW::Monitor::getPositionImpl(MonitorHandle mon){
	if(isValidImpl(mon)){
		Math::Vec2i result;
		glfwGetMonitorPos(mon, &result.x, &result.y);
		return result;
	} else {
		return {};
	}
}

Math::Vec2d GLFW::Monitor::getPhysicalSizeImpl(MonitorHandle mon){
	if(isValidImpl(mon)){
		Math::Vec2i result;
		glfwGetMonitorPhysicalSize(mon, &result.x, &result.y);
		return Math::Vec2d(result) * 1e-3;
	} else {
		return {};
	}
}

GLFW::Monitor::Mode GLFW::Monitor::getModeImpl(MonitorHandle mon){
	if(isValidImpl(mon)){
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
	if(isValidImpl(mon)){
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
	: m_window(s_thread->execute<WindowHandle>(createWindow,
		size, name, this
	))
{
	if(m_window == nullptr){
		throw Exception("Error creating the GLFW window");
	}
}

GLFW::Window::Window(Window&& other) {
	std::lock_guard<std::mutex> lock(GLFW::s_cbkMutex);

	m_window = other.m_window;
	other.m_window = nullptr;

	glfwSetWindowUserPointer(m_window, this); //Thread-safe GLFW function
	m_callbacks = std::move(other.m_callbacks);
}

GLFW::Window::~Window(){
	if(m_window){
		s_thread->execute<void>(destroyWindow, m_window);
	}
}

GLFW::Window& GLFW::Window::operator=(Window&& other){
	if(m_window){
		s_thread->execute<void>(destroyWindow, m_window);
	}

	std::lock_guard<std::mutex> lock(GLFW::s_cbkMutex);

	m_callbacks = std::move(other.m_callbacks);

	m_window = other.m_window;
	other.m_window = nullptr;

	glfwSetWindowUserPointer(m_window, this); //Thread-safe GLFW function

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
	VkResult err = glfwCreateWindowSurface( //Thread-safe GLFW function
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
	s_thread->execute<void>(setNameImpl, m_window, name);
}



void GLFW::Window::setState(State st){
	s_thread->execute<void>(setStateImpl, m_window, &m_windowedState, st);
}

GLFW::Window::State GLFW::Window::getState() const{
	return s_thread->execute<State>(getStateImpl, m_window);
}

void GLFW::Window::setStateCallback(StateCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_cbkMutex);
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
	s_thread->execute<void>(setMonitorImpl, m_window, &m_windowedState, mon, mode);
}

GLFW::Monitor GLFW::Window::getMonitor() const{
	return Monitor(s_thread->execute<Monitor>(getMonitorImpl, m_window));
}



void GLFW::Window::setPosition(const Math::Vec2i& pos){
	s_thread->execute<void>(setPositionImpl, m_window, pos);
}

Math::Vec2i GLFW::Window::getPosition() const{
	return s_thread->execute<Math::Vec2i>(getPositionImpl, m_window);
}

void GLFW::Window::setPositionCallback(PositionCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_cbkMutex);
	m_callbacks.positionCbk = std::move(cbk);
}

const GLFW::Window::PositionCallback& GLFW::Window::getPositionCallback() const{
	return m_callbacks.positionCbk;
}



void GLFW::Window::setSize(const Math::Vec2i& size){
	s_thread->execute<void>(setSizeImpl, m_window, size);
}

Math::Vec2i GLFW::Window::getSize() const{
	return s_thread->execute<Math::Vec2i>(getSizeImpl, m_window);
}

void GLFW::Window::setSizeCallback(SizeCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_cbkMutex);
	m_callbacks.sizeCbk = std::move(cbk);
}

const GLFW::Window::SizeCallback& GLFW::Window::getSizeCallback() const{
	return m_callbacks.sizeCbk;
}



void GLFW::Window::setOpacity(float opa){
	s_thread->execute<void>(setOpacityImpl, m_window, opa);
}

float GLFW::Window::getOpacity() const{
	return s_thread->execute<float>(getOpacityImpl, m_window);
}



Resolution GLFW::Window::getResolution() const{
	return s_thread->execute<Resolution>(getResolutionImpl, m_window);
}

void GLFW::Window::setResolutionCallback(ResolutionCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_cbkMutex);
	m_callbacks.resolutionCbk = std::move(cbk);
}

const GLFW::Window::ResolutionCallback& GLFW::Window::getResolutionCallback() const{
	return m_callbacks.resolutionCbk;
}



Math::Vec2f GLFW::Window::getScale() const{
	return s_thread->execute<Math::Vec2f>(getScaleImpl, m_window);
}

void GLFW::Window::setScaleCallback(ScaleCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_cbkMutex);
	m_callbacks.scaleCbk = std::move(cbk);
}

const GLFW::Window::ScaleCallback& GLFW::Window::getScaleCallback() const{
	return m_callbacks.scaleCbk;
}



void GLFW::Window::close(){
	s_thread->execute<void>(destroyWindow, m_window);
	m_window = nullptr;
}

bool GLFW::Window::shouldClose() const{
	return glfwWindowShouldClose(m_window); //Thread-safe GLFW function
}

void GLFW::Window::setCloseCallback(CloseCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_cbkMutex);
	m_callbacks.closeCbk = std::move(cbk);
}

const GLFW::Window::CloseCallback& GLFW::Window::getCloseCallback() const{
	return m_callbacks.closeCbk;
}



void GLFW::Window::focus(){
	s_thread->execute<void>(focusImpl, m_window);
}

void GLFW::Window::setFocusCallback(FocusCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_cbkMutex);
	m_callbacks.focusCbk = std::move(cbk);
}

const GLFW::Window::FocusCallback& GLFW::Window::getFocusCallback() const{
	return m_callbacks.focusCbk;
}



void GLFW::Window::setRefreshCallback(RefreshCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::s_cbkMutex);
	m_callbacks.refreshCbk = std::move(cbk);
}

const GLFW::Window::RefreshCallback& GLFW::Window::getRefreshCallback() const{
	return m_callbacks.refreshCbk;
}


//Create / Destroy
GLFW::WindowHandle GLFW::Window::createWindow(	const Math::Vec2i& size, 
												const std::string_view& name,
												Window* usrPtr )
{
	//Set Vulkan compatibility
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Create the window
	WindowHandle win = glfwCreateWindow(
		size.x, size.y,
		name.data(),
		static_cast<MonitorHandle>(nullptr),
		static_cast<WindowHandle>(nullptr)
	);

	if(win == nullptr){
		return nullptr;
	}

	//Set all callbacks
	glfwSetWindowUserPointer(win, static_cast<void*>(usrPtr));
	glfwSetWindowPosCallback(win, positionCbk);
	glfwSetWindowSizeCallback(win, sizeCbk);
	glfwSetWindowCloseCallback(win, closeCbk);
	glfwSetWindowRefreshCallback(win, refreshCbk);
	glfwSetWindowFocusCallback(win, focusCbk);
	glfwSetWindowIconifyCallback(win, iconifyCbk);
	glfwSetWindowMaximizeCallback(win, maximizeCbk);
	glfwSetFramebufferSizeCallback(win, framebufferCbk);
	glfwSetWindowContentScaleCallback(win, scaleCbk);


	s_thread->m_windowCount.fetch_add(1);
	return win;
}

void GLFW::Window::destroyWindow(WindowHandle window) {
	glfwDestroyWindow(window);
	s_thread->m_windowCount.fetch_sub(1);
}


//Function implementations
void GLFW::Window::setNameImpl(WindowHandle win, const std::string_view& name){
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
				Monitor(glfwGetPrimaryMonitor()),
				Monitor::getModeImpl(glfwGetPrimaryMonitor())
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
									const Monitor& newMon, 
									const Monitor::Mode& mode )
{
	const auto oldMonHandle = getMonitorImpl(win).m_monitor;
	const auto newMonHandle = newMon.m_monitor;

	if(newMonHandle != oldMonHandle){
		//State has changed. Evaluate what to do
		if(Monitor::isValidImpl(newMonHandle)) {
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



void GLFW::Window::setPositionImpl(WindowHandle win, const Math::Vec2i& pos){
	glfwSetWindowPos(win, pos.x, pos.y);
}

Math::Vec2i GLFW::Window::getPositionImpl(WindowHandle win){
	Math::Vec2i result;
	glfwGetWindowPos(win, &result.x, &result.y);
	return result;
}



void GLFW::Window::setSizeImpl(WindowHandle win, const Math::Vec2i& size){
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


//Callbacks
void GLFW::Window::positionCbk(WindowHandle win, int x, int y){
	if(s_thread->getCallbacksEnabled()) {
		std::lock_guard<std::mutex> lock(s_cbkMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.positionCbk){
			window->m_callbacks.positionCbk(Math::Vec2i(x, y));
		}
	}
}

void GLFW::Window::sizeCbk(WindowHandle win, int x, int y){
	if(s_thread->getCallbacksEnabled()) {
		std::lock_guard<std::mutex> lock(s_cbkMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.sizeCbk){
			window->m_callbacks.sizeCbk(Math::Vec2i(x, y));
		}
	}
}

void GLFW::Window::closeCbk(WindowHandle win){
	if(s_thread->getCallbacksEnabled()) {
		std::lock_guard<std::mutex> lock(s_cbkMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.closeCbk){
			window->m_callbacks.closeCbk();
		}
	}
}

void GLFW::Window::refreshCbk(WindowHandle win){
	if(s_thread->getCallbacksEnabled()) {
		std::lock_guard<std::mutex> lock(s_cbkMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.refreshCbk){
			window->m_callbacks.refreshCbk();
		}
	}
}

void GLFW::Window::focusCbk(WindowHandle win, int x){
	if(s_thread->getCallbacksEnabled()) {
		std::lock_guard<std::mutex> lock(s_cbkMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.focusCbk){
			window->m_callbacks.focusCbk(static_cast<bool>(x));
		}
	}
}

void GLFW::Window::iconifyCbk(WindowHandle win, int x){
	if(s_thread->getCallbacksEnabled()) {
		std::lock_guard<std::mutex> lock(s_cbkMutex);

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

void GLFW::Window::maximizeCbk(WindowHandle win, int x){
	if(s_thread->getCallbacksEnabled()) {
		std::lock_guard<std::mutex> lock(s_cbkMutex);

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

void GLFW::Window::framebufferCbk(WindowHandle win, int x, int y){
	if(s_thread->getCallbacksEnabled()) {
		std::lock_guard<std::mutex> lock(s_cbkMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.resolutionCbk){
			window->m_callbacks.resolutionCbk(Resolution(x, y));
		}
	}
}

void GLFW::Window::scaleCbk(WindowHandle win, float x, float y){
	if(s_thread->getCallbacksEnabled()) {
		std::lock_guard<std::mutex> lock(s_cbkMutex);

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

		if(window->m_callbacks.scaleCbk){
			window->m_callbacks.scaleCbk(Math::Vec2f(x, y));
		}
	}
}

/*
 * GLFW::Thread
 */

GLFW::Thread::Thread()
	: m_exit(false)
	, m_windowCount(0)
	, m_cbkEnabled(false)

{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_thread = std::thread(&Thread::threadFunc, this);
	m_completeCondition.wait(lock);
}

GLFW::Thread::~Thread() {
	std::unique_lock<std::mutex> lock(m_mutex);
	m_exit.store(true);
	threadContinue();
	lock.unlock();

	m_thread.join();
}

template<typename Ret, typename Func, typename... Args>
Ret GLFW::Thread::execute(Func&& func, Args&&... args) {
	if(std::this_thread::get_id() == m_thread.get_id()){
		return func(std::forward<Args>(args)...); //We are on the main thread. Simply execute it
	}else {
		std::unique_lock<std::mutex> lock(m_mutex);

		//Create a future object to pass it to the main thread
		auto futur = std::async(
			std::launch::deferred, 
			std::forward<Func>(func), 
			std::forward<Args>(args)...
		);
		m_executions.emplace([&futur] () -> void { futur.wait(); });
		
		//Wait until execution is complete
		threadContinue();
		m_completeCondition.wait(lock);

		return futur.get();
	}
}

bool GLFW::Thread::getCallbacksEnabled() const {
	return m_cbkEnabled.load();
}

void GLFW::Thread::threadFunc(){
	std::unique_lock<std::mutex> lock(m_mutex);

	glfwInit();

	glfwSetMonitorCallback(monitorCbk);

	while(m_exit.load() == false){
		//Notify waiting threads
		m_completeCondition.notify_all();

		//Wait until an event arises
		if(m_windowCount.load() > 0){
			lock.unlock();
			m_cbkEnabled.store(true);

			glfwWaitEvents();

			m_cbkEnabled.store(false);
			lock.lock();
		} else {
			m_continueCondition.wait(lock);
		}

		//Execute all pending executions
		while(m_executions.size() > 0){
			m_executions.front()();
			m_executions.pop();
		}
	}

	glfwTerminate();
}

void GLFW::Thread::threadContinue(){
	if(m_windowCount.load() > 0){
		glfwPostEmptyEvent();
	} else {
		m_continueCondition.notify_all();
	}
}


}