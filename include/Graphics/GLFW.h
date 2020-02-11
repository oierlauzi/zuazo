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

#define GLFW_INCLUDE_NONE //Don't include GL
#include <GLFW/glfw3.h>

namespace Zuazo::Graphics{

class GLFW {
public:
	class Monitor;
	class Window;

	GLFW();
	GLFW(const GLFW& other) = delete;
	~GLFW();

	GLFW& operator=(const GLFW& other) = delete;

	static std::mutex&							getCallbackMutex();

	static const std::vector<Monitor>& 			getMonitors();

	static std::vector<vk::ExtensionProperties> getRequiredVulkanExtensions();
	static bool									getPresentationSupport(	const vk::Instance& instance, 
																		const vk::PhysicalDevice& device, 
																		uint32_t family);
	static std::vector<uint32_t>				getPresentationQueueFamilies(	const vk::Instance& instance, 
																				const vk::PhysicalDevice& device, 
																				uint32_t queueCount);

	static const Monitor NO_MONITOR;

private:
	static std::atomic<size_t>		s_instanceCount;
	static std::atomic<size_t>		s_windowCount;

	static std::thread				s_thread;
	static std::atomic<bool>		s_threadExit;
	static std::mutex				s_threadMutex;
	static std::condition_variable	s_threadContinueCondition;
	static std::condition_variable	s_threadCompleteCondition;
	static std::queue<std::function<void(void)>> s_threadExecutions;

	static std::mutex				s_cbkMutex;
	static std::atomic<bool>		s_enableCbks;

	static std::vector<Monitor>		s_monitors;

	static void 					initialize();
	static void 					terminate();

	static void						threadFunc();
	static void						threadContinue();
	template<typename T, typename... Args>
	static T 						threadExecute(std::function<T(Args...)> func, Args... args);

	static void						addMonitor(GLFWmonitor* mon);
	static void						eraseMonitor(GLFWmonitor* mon);
	static const Monitor&			getMonitor(GLFWmonitor* mon);

	static void						monitorCbk(GLFWmonitor* mon, int evnt);
	static void						positionCbk(GLFWwindow* win, int x, int y);
	static void						sizeCbk(GLFWwindow* win, int x, int y);
	static void						closeCbk(GLFWwindow* win);
	static void						refreshCbk(GLFWwindow* win);
	static void						focusCbk(GLFWwindow* win, int x);
	static void						iconifyCbk(GLFWwindow* win, int x);
	static void						maximizeCbk(GLFWwindow* win, int x);
	static void						framebufferCbk(GLFWwindow* win, int x, int y);
	static void						scaleCbk(GLFWwindow* win, float x, float y);
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
	Monitor(const Monitor& other) = delete; 
	Monitor(Monitor&& other);
	~Monitor() = default;

	Monitor&							operator=(const Monitor& other) = delete;
	Monitor&							operator=(Monitor&& other);

	operator bool() const;

	std::string                         getName() const;
	Math::Vec2i                         getPosition() const;
	Math::Vec2d                         getPhysicalSize() const;
	Mode                                getMode() const;
	std::vector<Mode>                   getModes() const;

private:
	Monitor(GLFWmonitor* mon);

	GLFWmonitor*                        m_monitor = nullptr;
};



class GLFW::Window {
	friend GLFW;
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

	Window(	const Math::Vec2i& size = { 640, 480 }, 
			const std::string_view& name = "" );
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
	const Monitor&				getMonitor() const;

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

	Callbacks					m_callbacks;	
	std::optional<WindowGeometry> m_windowedState;

	GLFWwindow* 				m_window = nullptr;

	static GLFWwindow*		createWindow(	int x, int y, 
											const char* name,
											void* usrPtr );
	static void				destroyWindow(GLFWwindow* window);

};

}