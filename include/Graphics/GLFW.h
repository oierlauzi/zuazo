#pragma once

#include "Vulkan.h"
#include "../Math/Vector.h"
#include "../Resolution.h"
#include "../Timing/Chrono.h"

#include <atomic>
#include <cstddef>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string_view>
#include <queue>
#include <optional>
#include <memory>

#define GLFW_INCLUDE_NONE //Don't include GL
#include <GLFW/glfw3.h>

namespace Zuazo::Graphics{

class GLFW {
public:
	class Monitor;
	class Window;

	enum class MonitorEvent {
		CONNECTED,
		DISCONNECTED
	};

	using MonitorCallback = std::function<void(Monitor, MonitorEvent)>;

	GLFW();
	GLFW(const GLFW& other) = delete;
	~GLFW();

	GLFW& operator=(const GLFW& other) = delete;

	static std::vector<Monitor>					getMonitors();
	static void									setMonitorCallback(MonitorCallback&& cbk);
	static const MonitorCallback&				getMonitorCallback();

	static std::vector<vk::ExtensionProperties> getRequiredVulkanExtensions();
	static bool									getPresentationSupport(	const vk::Instance& instance, 
																		const vk::PhysicalDevice& device, 
																		uint32_t family);
	static std::vector<uint32_t>				getPresentationQueueFamilies(	const vk::Instance& instance, 
																				const vk::PhysicalDevice& device, 
																				uint32_t queueCount);

	static const Monitor NO_MONITOR;

private:
	using MonitorHandle = GLFWmonitor*;
	using WindowHandle = GLFWwindow*;

	class Thread;

	static std::atomic<size_t>			s_instanceCount;
	static std::unique_ptr<Thread>		s_thread;
	static std::mutex					s_cbkMutex;

	static MonitorCallback				s_monitorCbk;
	static MonitorHandle				s_activeMonitorCbk;

	static void 						initialize();
	static void 						terminate();

	static std::vector<Monitor>			getMonitorsImpl();

	static void							monitorCbk(MonitorHandle mon, int evnt);
};


class GLFW::Monitor {
	friend GLFW;
	friend GLFW::Window;
public:
	using ColorDepth = Math::Vec3i;

	struct Mode {
		ColorDepth colorDepth;
		Math::Vec2i size;
		Timing::Rate frameRate;
	};

	Monitor() = default;
	Monitor(const Monitor& other) = default; 
	~Monitor() = default;

	Monitor&							operator=(const Monitor& other) = default;

	operator bool() const;

	std::string                         getName() const;
	Math::Vec2i                         getPosition() const;
	Math::Vec2d                         getPhysicalSize() const;
	Mode                                getMode() const;
	std::vector<Mode>                   getModes() const;

private:
	Monitor(MonitorHandle mon);

	MonitorHandle						m_monitor = nullptr;

	static bool							isValidImpl(MonitorHandle mon);
	static std::string					getNameImpl(MonitorHandle mon);
	static Math::Vec2i					getPositionImpl(MonitorHandle mon);
	static Math::Vec2d					getPhysicalSizeImpl(MonitorHandle mon);
	static Mode							getModeImpl(MonitorHandle mon);
	static std::vector<Mode>			getModesImpl(MonitorHandle mon);

};



class GLFW::Window {
public:
	enum class State {
		NORMAL,
		HIDDEN,
		FULLSCREEN,
		ICONIFIED,
		MAXIMIZED
	};

	using StateCallback = std::function<void(State)>;
	using PositionCallback = std::function<void(Math::Vec2i)>;
	using SizeCallback = std::function<void(Math::Vec2i)>;
	using ResolutionCallback = std::function<void(Resolution)>;
	using ScaleCallback = std::function<void(Math::Vec2f)>;
	using CloseCallback = std::function<void()>;
	using RefreshCallback = std::function<void()>;
	using FocusCallback = std::function<void(bool)>;

	Window() = default;
	Window(	const Math::Vec2i& size, 
			const std::string_view& name );
	Window(const Window& other) = delete;
	Window(Window&& other);
	~Window();

	Window& operator=(const Window& other) = delete;
	Window& operator=(Window&& other);

	operator bool() const;

	vk::UniqueSurfaceKHR 		getSurface(const Vulkan& vulkan) const;

	void						setName(const std::string_view& name);

	void						setState(State st);
	State						getState() const;
	void						setStateCallback(StateCallback&& cbk);
	const StateCallback&		getStateCallback() const;

	void						setMonitor(const Monitor& mon);
	void						setMonitor(const Monitor& mon, const Monitor::Mode& mode);
	Monitor						getMonitor() const;

	void						setPosition(const Math::Vec2i& pos);
	Math::Vec2i					getPosition() const;
	void						setPositionCallback(PositionCallback&& cbk);
	const PositionCallback&		getPositionCallback() const;

	void						setSize(const Math::Vec2i& size);
	Math::Vec2i					getSize() const;
	void						setSizeCallback(SizeCallback&& cbk);
	const SizeCallback&			getSizeCallback() const;


	void						setOpacity(float opa);
	float						getOpacity() const;

	Resolution					getResolution() const;
	void						setResolutionCallback(ResolutionCallback&& cbk);
	const ResolutionCallback&	getResolutionCallback() const;

	Math::Vec2f					getScale() const;
	void						setScaleCallback(ScaleCallback&& cbk);
	const ScaleCallback&		getScaleCallback() const;

	void						close();
	bool						shouldClose() const;
	void						setCloseCallback(CloseCallback&& cbk);
	const CloseCallback&		getCloseCallback() const;

	void						focus();
	void						setFocusCallback(FocusCallback&& cbk);
	const FocusCallback&		getFocusCallback() const;

	void						setRefreshCallback(RefreshCallback&& cbk);
	const RefreshCallback&		getRefreshCallback() const;


private:
	struct WindowGeometry {
		Math::Vec2i pos, size;
	};

	struct Callbacks {
		StateCallback				stateCbk;
		PositionCallback			positionCbk;
		SizeCallback				sizeCbk;
		ResolutionCallback			resolutionCbk;
		ScaleCallback				scaleCbk;
		CloseCallback				closeCbk;
		RefreshCallback				refreshCbk;
		FocusCallback				focusCbk;
	};

	Callbacks						m_callbacks;	
	WindowGeometry 					m_windowedState = {};

	WindowHandle 					m_window = nullptr;

	static WindowHandle				createWindow(	const Math::Vec2i& size, 
													const std::string_view& name,
													Window* usrPtr );
	static void						destroyWindow(WindowHandle window);

	static void						setNameImpl(WindowHandle win, const std::string_view& name);

	static void						setStateImpl(	WindowHandle win,
													WindowGeometry* windowedGeom,
													State st );
	static State					getStateImpl(WindowHandle win);

	static void						setMonitorImpl(	WindowHandle win, 
													WindowGeometry* windowedGeom,
													const Monitor& newMon, 
													const Monitor::Mode& mode );
	static Monitor					getMonitorImpl(WindowHandle win);

	static void						setPositionImpl(WindowHandle win, const Math::Vec2i& pos);
	static Math::Vec2i				getPositionImpl(WindowHandle win);

	static void						setSizeImpl(WindowHandle win, const Math::Vec2i& size);
	static Math::Vec2i				getSizeImpl(WindowHandle win);

	static void						setOpacityImpl(WindowHandle win, float opa);
	static float					getOpacityImpl(WindowHandle win);

	static Resolution				getResolutionImpl(WindowHandle win);

	static Math::Vec2f				getScaleImpl(WindowHandle win);

	static void						focusImpl(WindowHandle win);

	static void						positionCbk(WindowHandle win, int x, int y);
	static void						sizeCbk(WindowHandle win, int x, int y);
	static void						closeCbk(WindowHandle win);
	static void						refreshCbk(WindowHandle win);
	static void						focusCbk(WindowHandle win, int x);
	static void						iconifyCbk(WindowHandle win, int x);
	static void						maximizeCbk(WindowHandle win, int x);
	static void						framebufferCbk(WindowHandle win, int x, int y);
	static void						scaleCbk(WindowHandle win, float x, float y);

};

class GLFW::Thread {
	friend GLFW::Window;
public:
	Thread();
	Thread(const Thread& other) = delete;
	~Thread();

	Thread& 						operator=(const Thread& other) = delete;

	template<typename Ret, typename Func, typename... Args>
	Ret 							execute(Func&& func, Args&&... args);

	bool							getCallbacksEnabled() const;
private:
	using QueueFunc = std::function<void(void)>;

	std::atomic<bool>				m_exit;
	std::atomic<size_t>				m_windowCount;
	std::atomic<bool>				m_cbkEnabled;

	std::thread						m_thread;
	std::mutex						m_mutex;
	std::condition_variable			m_continueCondition;
	std::condition_variable			m_completeCondition;
	std::queue<QueueFunc> 			m_executions;

	void							threadFunc();
	void							threadContinue();

};

}