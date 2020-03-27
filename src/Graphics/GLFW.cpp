#include <Graphics/GLFW.h>

#include <Exception.h>

#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <type_traits>

#define GLFW_INCLUDE_NONE //Don't include GL
#include <GLFW/glfw3.h>

namespace Zuazo::Graphics {

/*
 * GLFW::Callbacks
 */
struct GLFW::Callbacks{
	static inline std::atomic<bool> enabled = true;
	static inline std::mutex mutex;
	static inline std::atomic<MonitorHandle> currentMonitor = nullptr;


	static void monitorCbk(MonitorHandle mon, int evnt){
		if(enabled) {
			std::lock_guard<std::mutex> lock(mutex);

			if(s_monitorCbk){
				currentMonitor = mon;

				switch(evnt){
				case GLFW_CONNECTED: 
					getMonitorCallback()(GLFW::Monitor(mon), GLFW::MonitorEvent::CONNECTED); 
					break;
				case GLFW_DISCONNECTED: 
					getMonitorCallback()(GLFW::Monitor(mon), GLFW::MonitorEvent::DISCONNECTED); 
					break;
				default:
					break;
				}

				currentMonitor = nullptr;
			}
		}
	}

	static void positionCbk(WindowHandle win, int x, int y){
		if(enabled) {
			std::lock_guard<std::mutex> lock(mutex);

			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

			if(window->m_callbacks.positionCbk){
				window->m_callbacks.positionCbk(Math::Vec2i(x, y));
			}
		}
	}

	static void sizeCbk(WindowHandle win, int x, int y){
		if(enabled) {
			std::lock_guard<std::mutex> lock(mutex);

			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

			if(window->m_callbacks.sizeCbk){
				window->m_callbacks.sizeCbk(Math::Vec2i(x, y));
			}
		}
	}

	static void closeCbk(WindowHandle win){
		if(enabled) {
			std::lock_guard<std::mutex> lock(mutex);

			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

			if(window->m_callbacks.closeCbk){
				window->m_callbacks.closeCbk();
			}
		}
	}

	static void refreshCbk(WindowHandle win){
		if(enabled) {
			std::lock_guard<std::mutex> lock(mutex);

			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

			if(window->m_callbacks.refreshCbk){
				window->m_callbacks.refreshCbk();
			}
		}
	}

	static void focusCbk(WindowHandle win, int x){
		if(enabled) {
			std::lock_guard<std::mutex> lock(mutex);

			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

			if(window->m_callbacks.focusCbk){
				window->m_callbacks.focusCbk(static_cast<bool>(x));
			}
		}
	}

	static void iconifyCbk(WindowHandle win, int x){
		if(enabled) {
			std::lock_guard<std::mutex> lock(mutex);

			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

			if(window->m_callbacks.stateCbk){
				if(x == GLFW_TRUE){
					window->m_callbacks.stateCbk(Window::State::ICONIFIED);
				} else {
					window->m_callbacks.stateCbk(Window::State::NORMAL);
				}
			}
		}
	}

	static void maximizeCbk(WindowHandle win, int x){
		if(enabled) {
			std::lock_guard<std::mutex> lock(mutex);

			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

			if(window->m_callbacks.stateCbk){
				if(x == GLFW_TRUE){
					window->m_callbacks.stateCbk(Window::State::MAXIMIZED);
				} else {
					window->m_callbacks.stateCbk(Window::State::NORMAL);
				}
			}
		}
	}

	static void framebufferCbk(WindowHandle win, int x, int y){
		if(enabled) {
			std::lock_guard<std::mutex> lock(mutex);

			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

			if(window->m_callbacks.resolutionCbk){
				window->m_callbacks.resolutionCbk(Resolution(x, y));
			}
		}
	}

	static void scaleCbk(WindowHandle win, float x, float y){
		if(enabled) {
			std::lock_guard<std::mutex> lock(mutex);

			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

			if(window->m_callbacks.scaleCbk){
				window->m_callbacks.scaleCbk(Math::Vec2f(x, y));
			}
		}
	}
};


/*
 * Impl
 */

struct GLFW::Impl {
	using QueueFunc = std::function<void(void)>;

	std::atomic<bool>				exit;
	std::atomic<size_t>				windowCount;	

	std::mutex						mutex;
	std::condition_variable			continueCondition;
	std::condition_variable			completeCondition;
	std::queue<QueueFunc> 			executions;
	std::thread						thread;

	Impl()
		: exit(false)
		, windowCount(0)
		, thread(&Impl::threadFunc, this)
	{
		//Wait for completion
		std::unique_lock<std::mutex> lock(mutex);
		threadContinue();
		completeCondition.wait(lock);
	}

	~Impl() {
		std::unique_lock<std::mutex> lock(mutex);
		exit.store(true);
		threadContinue();
		lock.unlock();

		thread.join();
	}



	void setCallbacksEnabled(bool ena){
		std::unique_lock<std::mutex> lock(mutex);
		threadContinue();
		Callbacks::enabled.store(ena); //Important to be after threadContinue()!
		completeCondition.wait(lock); //Wait for any cbk to finish
	}

	bool getCallbacksEnabled() const {
		return Callbacks::enabled.load();
	}



	//Monitor stuff
	std::vector<Monitor> getMonitors(){
		return execute(getMonitorsImpl);
	}

	bool isValid(MonitorHandle mon){
		return execute(isValidImpl, mon);
	}

	std::string getName(MonitorHandle mon){
		return execute(getNameImpl, mon);
	}

	Math::Vec2i getPosition(MonitorHandle mon){
		return execute(getMonitorPositionImpl, mon);
	}

	Math::Vec2d getPhysicalSize(MonitorHandle mon){
		return execute(getPhysicalSizeImpl, mon);
	}

	Monitor::Mode getMode(MonitorHandle mon){
		return execute(getModeImpl, mon);
	}

	std::vector<Monitor::Mode> getModes(MonitorHandle mon){
		return execute(getModesImpl, mon);
	}


	//Window stuff
	GLFW::WindowHandle createWindow(const Math::Vec2i& size, 
									std::string_view name,
									Window* usrPtr )
	{
		return execute(
			std::mem_fn(&Impl::createWindowImpl),
			this, size, name, usrPtr
		);
	}

	void destroyWindow(WindowHandle window) {
		execute(
			std::mem_fn(&Impl::destroyWindowImpl),
			this, window
		);
	}

	void setName(WindowHandle win, std::string_view name){
		execute(setNameImpl, win, name);
	}



	void setState(	WindowHandle win,  
					Window::Geometry* windowedGeom,
					Window::State st )
	{
		execute(setStateImpl, win, windowedGeom, st);
	}

	Window::State getState(WindowHandle win){
		return execute(getStateImpl, win);
	}



	void setMonitor(WindowHandle win, 
					Window::Geometry* windowedGeom,
					const Monitor& newMon, 
					const Monitor::Mode& mode )
	{
		execute(setMonitorImpl, win, windowedGeom, newMon, mode);
	}

	Monitor getMonitor(WindowHandle win){
		return execute(getMonitorImpl, win);
	}



	void setPosition(WindowHandle win, Math::Vec2i pos){
		execute(setPositionImpl, win, pos);
	}

	Math::Vec2i getPosition(WindowHandle win){
		return execute(getWindowPositionImpl, win);
	}



	void setSize(WindowHandle win, Math::Vec2i size){
		execute(setSizeImpl, win, size);
	}

	Math::Vec2i getSize(WindowHandle win){
		return execute(getSizeImpl, win);
	}



	void setOpacity(WindowHandle win, float opa){
		execute(setOpacityImpl, win, opa);
	}

	float getOpacity(WindowHandle win){
		return execute(getOpacityImpl, win);
	}



	Resolution getResolution(WindowHandle win){
		return execute(getResolutionImpl, win);
	}



	Math::Vec2f getScale(WindowHandle win){
		return execute(getScaleImpl, win);
	}


	void focus(WindowHandle win){
		execute(focusImpl, win);
	}



	void setDecorated(WindowHandle win, bool deco){
		execute(setDecoratedImpl, win, deco);
	}

	bool getDecorated(WindowHandle win){
		return execute(getDecoratedImpl, win);
	}



	void setResizeable(WindowHandle win, bool resizeable){
		execute(setResizeableImpl, win, resizeable);
	}

	bool getResizeable(WindowHandle win){
		return execute(getResizeableImpl, win);
	}

private:
	void threadFunc(){
		std::unique_lock<std::mutex> lock(mutex);

		glfwInit();

		glfwSetMonitorCallback(Callbacks::monitorCbk);

		while(exit.load() == false){
			//Notify waiting threads
			completeCondition.notify_all();

			//Wait until an event arises
			if(windowCount.load() > 0 && Callbacks::enabled.load()){
				lock.unlock();
				glfwWaitEvents();
				lock.lock();
			} else {
				continueCondition.wait(lock);
			}

			//Execute all pending executions
			while(executions.size() > 0){
				executions.front()();
				executions.pop();
			}
		}

		glfwTerminate();
	}

	void threadContinue(){
		if(windowCount.load() > 0 && Callbacks::enabled.load()){
			glfwPostEmptyEvent();
		} else {
			continueCondition.notify_all();
		}
	}

	template<typename Func, typename... Args>
	typename std::invoke_result<Func, Args...>::type execute(Func&& func, Args&&... args) {
		using Ret = typename std::invoke_result<Func, Args...>::type;

		if(std::this_thread::get_id() == thread.get_id()){
			return func(std::forward<Args>(args)...); //We are on the main thread. Simply execute it
		}else {
			std::unique_lock<std::mutex> lock(mutex);

			//Create a future object to pass it to the main thread
			std::packaged_task<Ret(Args...)> task(std::forward<Func>(func));
			executions.emplace([&] { task(std::forward<Args>(args)...); });
			
			//Wait until execution is complete
			threadContinue();
			completeCondition.wait(lock);

			return task.get_future().get();
		}
	}



	//Monitor implementations
	static std::vector<Monitor> getMonitorsImpl(){
		int cnt;
		MonitorHandle* monitors = glfwGetMonitors(&cnt);

		std::vector<GLFW::Monitor> result(cnt);
		for(size_t i = 0; i < result.size(); i++){
			result[i] = Monitor(monitors[i]);
		}

		return result;
	}

	static bool isValidImpl(MonitorHandle mon){
		if(Callbacks::currentMonitor == mon){
			return true;
		}

		int cnt;
		MonitorHandle* start = glfwGetMonitors(&cnt);
		MonitorHandle* end = start + cnt;

		return std::find(start, end + cnt, mon) != end;
	}

	static std::string getNameImpl(MonitorHandle mon){
		if(isValidImpl(mon)){
			return std::string(glfwGetMonitorName(mon));
		} else {
			return {};
		}
	}

	static Math::Vec2i getMonitorPositionImpl(MonitorHandle mon){
		if(isValidImpl(mon)){
			Math::Vec2i result;
			glfwGetMonitorPos(mon, &result.x, &result.y);
			return result;
		} else {
			return {};
		}
	}

	static Math::Vec2d getPhysicalSizeImpl(MonitorHandle mon){
		if(isValidImpl(mon)){
			Math::Vec2i result;
			glfwGetMonitorPhysicalSize(mon, &result.x, &result.y);
			return Math::Vec2d(result) * 1e-3;
		} else {
			return {};
		}
	}

	static Monitor::Mode getModeImpl(MonitorHandle mon){
		if(isValidImpl(mon)){
			const GLFWvidmode* mod = glfwGetVideoMode(mon);

			return Monitor::Mode{
				Monitor::ColorDepth(mod->redBits, mod->greenBits, mod->blueBits),
				Math::Vec2i(mod->width, mod->height),
				Timing::Rate(mod->refreshRate, 1)
			};
		} else {
			return {};
		}
	}

	static std::vector<Monitor::Mode> getModesImpl(MonitorHandle mon){
		if(isValidImpl(mon)){
			int cnt;
			const GLFWvidmode* mod = glfwGetVideoModes(mon, &cnt);

			std::vector<Monitor::Mode> modes; modes.reserve(cnt);
			for(int i = 0; i < cnt; i++){
				modes.emplace_back(Monitor::Mode{
					Monitor::ColorDepth(mod[i].redBits, mod[i].greenBits, mod[i].blueBits),
					Math::Vec2i(mod[i].width, mod[i].height),
					Timing::Rate(mod[i].refreshRate, 1)
				});
			}

			return modes;
		} else {
			return {};
		}
	}



	//Window implementations
	GLFW::WindowHandle createWindowImpl(const Math::Vec2i& size, 
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
		glfwSetWindowPosCallback(win, Callbacks::positionCbk);
		glfwSetWindowSizeCallback(win, Callbacks::sizeCbk);
		glfwSetWindowCloseCallback(win, Callbacks::closeCbk);
		glfwSetWindowRefreshCallback(win, Callbacks::refreshCbk);
		glfwSetWindowFocusCallback(win, Callbacks::focusCbk);
		glfwSetWindowIconifyCallback(win, Callbacks::iconifyCbk);
		glfwSetWindowMaximizeCallback(win, Callbacks::maximizeCbk);
		glfwSetFramebufferSizeCallback(win, Callbacks::framebufferCbk);
		glfwSetWindowContentScaleCallback(win, Callbacks::scaleCbk);


		windowCount.fetch_add(1);
		return win;
	}

	void destroyWindowImpl(WindowHandle window) {
		glfwDestroyWindow(window);
		windowCount.fetch_sub(1);
	}

	static void setNameImpl(WindowHandle win, std::string_view name){
		glfwSetWindowTitle(win, name.data());
	}



	static void setStateImpl(	WindowHandle win,  
								Window::Geometry* windowedGeom,
								Window::State st )
	{
		const auto lastState = getStateImpl(win);

		if(lastState != st){
			//Leave it on normal state
			switch (lastState) {
			case Window::State::HIDDEN:
				glfwShowWindow(win);
				break;

			case Window::State::FULLSCREEN:
				setMonitorImpl(	
					win, 
					windowedGeom, 
					GLFW::NO_MONITOR, 
					Monitor::Mode()
				);
				break;

			case Window::State::ICONIFIED:
				glfwRestoreWindow(win);
				break;

			case Window::State::MAXIMIZED:
				glfwRestoreWindow(win);
				break;
			
			default: //State::NORMAL
				break;
			}

			//Switch to the desired state
			switch (st) {
			case Window::State::HIDDEN:
				glfwHideWindow(win);
				break;

			case Window::State::FULLSCREEN:
				setMonitorImpl(
					win, 
					windowedGeom, 
					Monitor(glfwGetPrimaryMonitor()),
					getModeImpl(glfwGetPrimaryMonitor())
				);
				break;

			case Window::State::ICONIFIED:
				glfwIconifyWindow(win);
				break;

			case Window::State::MAXIMIZED:
				glfwMaximizeWindow(win);
				break;
			
			default: //State::NORMAL
				break;
			}
		}
	}

	static Window::State getStateImpl(WindowHandle win){
		Window::State result;

		if(!glfwGetWindowAttrib(win, GLFW_VISIBLE)){
			result = Window::State::HIDDEN;
		} else if(glfwGetWindowMonitor(win)){
			result = Window::State::FULLSCREEN;
		}else if(glfwGetWindowAttrib(win, GLFW_ICONIFIED)){
			result = Window::State::ICONIFIED;
		} else if(glfwGetWindowAttrib(win, GLFW_MAXIMIZED)){
			result = Window::State::MAXIMIZED;
		} else {
			result = Window::State::NORMAL;
		}

		return result;
	}



	static void setMonitorImpl(	WindowHandle win, 
								Window::Geometry* windowedGeom,
								const Monitor& newMon, 
								const Monitor::Mode& mode )
	{
		const auto oldMonHandle = glfwGetWindowMonitor(win);
		const auto newMonHandle = newMon.m_monitor;

		if(newMonHandle != oldMonHandle){
			//State has changed. Evaluate what to do
			if(isValidImpl(newMonHandle)) {
				//It is going to be full-screen
				if(!oldMonHandle){
					//It was windowed. Save its state
					*windowedGeom = {
						getWindowPositionImpl(win),
						getSizeImpl(win)
					};
				}

				const Math::Vec2i pos = getMonitorPositionImpl(newMonHandle);

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

	static Monitor getMonitorImpl(WindowHandle win){
		return Monitor(glfwGetWindowMonitor(win));
	}



	static void setPositionImpl(WindowHandle win, const Math::Vec2i& pos){
		glfwSetWindowPos(win, pos.x, pos.y);
	}

	static Math::Vec2i getWindowPositionImpl(WindowHandle win){
		Math::Vec2i result;
		glfwGetWindowPos(win, &result.x, &result.y);
		return result;
	}



	static void setSizeImpl(WindowHandle win, const Math::Vec2i& size){
		glfwSetWindowSize(win, size.x, size.y);
	}

	static Math::Vec2i getSizeImpl(WindowHandle win){
		Math::Vec2i result;
		glfwGetWindowSize(win, &result.x, &result.y);
		return result;
	}



	static void setOpacityImpl(WindowHandle win, float opa){
		glfwSetWindowOpacity(win, opa);
	}

	static float getOpacityImpl(WindowHandle win){
		return glfwGetWindowOpacity(win);
	}



	static Resolution getResolutionImpl(WindowHandle win){
		int x, y;
		glfwGetFramebufferSize(win, &x, &y);
		return Resolution(x, y);
	}



	static Math::Vec2f getScaleImpl(WindowHandle win){
		Math::Vec2f result;
		glfwGetWindowContentScale(win, &result.x, &result.y);
		return result;
	}


	static void focusImpl(WindowHandle win){
		glfwFocusWindow(win);
	}



	static void setDecoratedImpl(WindowHandle win, bool deco){
		glfwSetWindowAttrib(win, GLFW_DECORATED, deco);
	}

	static bool getDecoratedImpl(WindowHandle win){
		return glfwGetWindowAttrib(win, GLFW_DECORATED);
	}



	static void setResizeableImpl(WindowHandle win, bool resizeable){
		glfwSetWindowAttrib(win, GLFW_RESIZABLE, resizeable);
	}

	static bool getResizeableImpl(WindowHandle win){
		return glfwGetWindowAttrib(win, GLFW_RESIZABLE);
	}

};


/*
 * GLFW
 */

const GLFW::Monitor GLFW::NO_MONITOR;
std::unique_ptr<GLFW::Impl> GLFW::s_impl;
std::atomic<size_t> GLFW::s_instanceCount(0);
GLFW::MonitorCallback GLFW::s_monitorCbk;



GLFW::GLFW(){
	if(s_instanceCount.fetch_add(1) == 0){
		//First instance. Initialze
		s_impl = std::make_unique<Impl>();
	}
}

GLFW::~GLFW(){
	if(s_instanceCount.fetch_sub(1) == 1){
		//Last instance. Teminate
		s_impl.reset();
	}
}

std::vector<GLFW::Monitor> GLFW::getMonitors(){
	return s_impl->getMonitors();
}

void GLFW::setMonitorCallback(MonitorCallback&& cbk){
	std::lock_guard<std::mutex> lock(Callbacks::mutex);
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

void GLFW::setCallbacksEnabled(bool ena){
	s_impl->setCallbacksEnabled(ena);
}

bool GLFW::getCallbacksEnabled(){
	return s_impl->getCallbacksEnabled();
}





/*
 * GLFW::Monitor
 */

GLFW::Monitor::Monitor(MonitorHandle mon) 
	: m_monitor(mon)
{
}

GLFW::Monitor::operator bool() const {
	return s_impl->isValid(m_monitor);
}

std::string GLFW::Monitor::getName() const {
	return s_impl->getName(m_monitor);
}

Math::Vec2i GLFW::Monitor::getPosition() const {
	return s_impl->getPosition(m_monitor);
}

Math::Vec2d GLFW::Monitor::getPhysicalSize() const {
	return s_impl->getPhysicalSize(m_monitor);
}

GLFW::Monitor::Mode GLFW::Monitor::getMode() const {
	return s_impl->getMode(m_monitor);
}

std::vector<GLFW::Monitor::Mode> GLFW::Monitor::getModes() const {
	return s_impl->getModes(m_monitor);
}



/*
 * GLFW::Window
 */

GLFW::Window::Window(	const  Math::Vec2i& size, 
						std::string_view name )
	: m_window(s_impl->createWindow(
		size, name, this
	))
{
	if(m_window == nullptr){
		throw Exception("Error creating the GLFW window");
	}
}

GLFW::Window::Window(Window&& other) {
	std::lock_guard<std::mutex> lock(GLFW::Callbacks::mutex);

	m_window = other.m_window;
	other.m_window = nullptr;

	glfwSetWindowUserPointer(m_window, this); //Thread-safe GLFW function
	m_callbacks = std::move(other.m_callbacks);
}

GLFW::Window::~Window(){
	if(m_window){
		s_impl->destroyWindow(m_window);
	}
}

GLFW::Window& GLFW::Window::operator=(Window&& other){
	if(m_window){
		s_impl->destroyWindow(m_window);
	}

	std::lock_guard<std::mutex> lock(GLFW::Callbacks::mutex);

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


void GLFW::Window::setName(std::string_view name){
	s_impl->setName(m_window, name);
}



void GLFW::Window::setState(State st){
	s_impl->setState(m_window, &m_windowedState, st);
}

GLFW::Window::State GLFW::Window::getState() const{
	return s_impl->getState(m_window);
}

void GLFW::Window::setStateCallback(StateCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::Callbacks::mutex);
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
	s_impl->setMonitor(m_window, &m_windowedState, mon, mode);
}

GLFW::Monitor GLFW::Window::getMonitor() const{
	return s_impl->getMonitor(m_window);
}



void GLFW::Window::setPosition(const Math::Vec2i& pos){
	s_impl->setPosition(m_window, pos);
}

Math::Vec2i GLFW::Window::getPosition() const{
	return s_impl->getPosition(m_window);
}

void GLFW::Window::setPositionCallback(PositionCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::Callbacks::mutex);
	m_callbacks.positionCbk = std::move(cbk);
}

const GLFW::Window::PositionCallback& GLFW::Window::getPositionCallback() const{
	return m_callbacks.positionCbk;
}



void GLFW::Window::setSize(const Math::Vec2i& size){
	s_impl->setSize(m_window, size);
}

Math::Vec2i GLFW::Window::getSize() const{
	return s_impl->getSize(m_window);
}

void GLFW::Window::setSizeCallback(SizeCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::Callbacks::mutex);
	m_callbacks.sizeCbk = std::move(cbk);
}

const GLFW::Window::SizeCallback& GLFW::Window::getSizeCallback() const{
	return m_callbacks.sizeCbk;
}



void GLFW::Window::setOpacity(float opa){
	s_impl->setOpacity(m_window, opa);
}

float GLFW::Window::getOpacity() const{
	return s_impl->getOpacity(m_window);
}



Resolution GLFW::Window::getResolution() const{
	return s_impl->getResolution(m_window);
}

void GLFW::Window::setResolutionCallback(ResolutionCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::Callbacks::mutex);
	m_callbacks.resolutionCbk = std::move(cbk);
}

const GLFW::Window::ResolutionCallback& GLFW::Window::getResolutionCallback() const{
	return m_callbacks.resolutionCbk;
}



Math::Vec2f GLFW::Window::getScale() const{
	return s_impl->getScale(m_window);
}

void GLFW::Window::setScaleCallback(ScaleCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::Callbacks::mutex);
	m_callbacks.scaleCbk = std::move(cbk);
}

const GLFW::Window::ScaleCallback& GLFW::Window::getScaleCallback() const{
	return m_callbacks.scaleCbk;
}



void GLFW::Window::close(){
	s_impl->destroyWindow(m_window);
	m_window = nullptr;
}

bool GLFW::Window::shouldClose() const{
	return glfwWindowShouldClose(m_window); //Thread-safe GLFW function
}

void GLFW::Window::setCloseCallback(CloseCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::Callbacks::mutex);
	m_callbacks.closeCbk = std::move(cbk);
}

const GLFW::Window::CloseCallback& GLFW::Window::getCloseCallback() const{
	return m_callbacks.closeCbk;
}



void GLFW::Window::focus(){
	s_impl->focus(m_window);
}

void GLFW::Window::setFocusCallback(FocusCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::Callbacks::mutex);
	m_callbacks.focusCbk = std::move(cbk);
}

const GLFW::Window::FocusCallback& GLFW::Window::getFocusCallback() const{
	return m_callbacks.focusCbk;
}



void GLFW::Window::setRefreshCallback(RefreshCallback&& cbk){
	std::lock_guard<std::mutex> lock(GLFW::Callbacks::mutex);
	m_callbacks.refreshCbk = std::move(cbk);
}

const GLFW::Window::RefreshCallback& GLFW::Window::getRefreshCallback() const{
	return m_callbacks.refreshCbk;
}



void GLFW::Window::setDecorated(bool deco){
	s_impl->setDecorated(m_window, deco);
}

bool GLFW::Window::getDecorated() const{
	return s_impl->getDecorated(m_window);
}



void GLFW::Window::setResizeable(bool resizeable){
	s_impl->setResizeable(m_window, resizeable);
}

bool GLFW::Window::getResizeable() const{
	return s_impl->getResizeable(m_window);
}

}