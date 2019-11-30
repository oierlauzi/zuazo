#pragma once

#include "Vulkan.h"
#include "../Math/Vector.h"
#include "../Timing/Chrono.h"
#include "../Resolution.h"
#include "../Zuazo.h"

#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional> 
#include <future>
#include <optional>
#include <queue>

#define GLFW_INCLUDE_NONE //Don't include GL
#include <GLFW/glfw3.h>

namespace Zuazo::Graphics {

class Window {
public:
	class Instance {
	public:
		Instance();
		Instance(const Instance& other) = delete;
		~Instance();
	private:
		static std::atomic<size_t>			s_instanceCount;
	};

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

	using StateCallback = std::function<void(State)>;
	using PositionCallback = std::function<void(Math::Vec2i)>;
	using SizeCallback = std::function<void(Math::Vec2i)>;
	using ResolutionCallback = std::function<void(Resolution)>;
	using ScaleCallback = std::function<void(Math::Vec2f)>;
	using CloseCallback = std::function<void()>;
	using RefreshCallback = std::function<void()>;
	using FocusCallback = std::function<void(bool)>;

	struct Callbacks {
		StateCallback                       stateCbk;
		PositionCallback                    positionCbk;
		SizeCallback                        sizeCbk;
		ResolutionCallback                  resolutionCbk;
		ScaleCallback                       scaleCbk;
		CloseCallback                       closeCbk;
		RefreshCallback                     refreshCbk;
		FocusCallback                       focusCbk;
	};

	Window() = default;
	Window(const  Math::Vec2i& size, std::string&& name = std::string(getApplicationInfo().name), const Monitor& mon = NO_MONITOR);
	Window(const Window& other) = delete;
	Window(Window&& other);
	virtual ~Window();

	Window&                             operator=(Window&& other);

	operator bool() const;


	vk::UniqueSurfaceKHR				getSurface(const vk::Instance& instance) const;				


	void                                setName(std::string&& name);
	const std::string&                  getName() const;

	void                                setState(State st);
	State                               getState() const;
	void                                setStateCallback(StateCallback&& cbk);
	const StateCallback&                getStateCallback() const;

	void                                setMonitor(const Monitor& mon);
	void                                setMonitor(const Monitor& mon, const Monitor::Mode& mode);
	const Monitor&                      getMonitor() const;

	void                                setPosition(const Math::Vec2i& pos);
	Math::Vec2i                         getPosition() const;
	void                                setPositionCallback(PositionCallback&& cbk);
	const PositionCallback&             getPositionCallback() const;

	void                                setSize(const Math::Vec2i& res);
	Math::Vec2i                         getSize() const;
	void                                setSizeCallback(SizeCallback&& cbk);
	const SizeCallback&                 getSizeCallback() const;

	void								setOpacity(float opa);
	float								getOpacity() const;

	Resolution                          getResolution() const;
	void                                setResolutionCallback(ResolutionCallback&& cbk);
	const ResolutionCallback&           getResolutionCallback() const;

	Math::Vec2f							getScale() const;
	void                                setScaleCallback(ScaleCallback&& cbk);
	const ScaleCallback&                getScaleCallback() const;

	void								close();
	bool								shouldClose() const;
	void                                setCloseCallback(CloseCallback&& cbk);
	const CloseCallback&                getCloseCallback() const;

	void								focus();
	void                                setFocusCallback(FocusCallback&& cbk);
	const FocusCallback&                getFocusCallback() const;

	void                                setRefreshCallback(RefreshCallback&& cbk);
	const RefreshCallback&              getRefreshCallback() const;

	void								setCallbacks(Callbacks&& cbks);
	const Callbacks&					getCallbacks() const;

	static const std::vector<Monitor>&  getMonitors();

	static std::vector<vk::ExtensionProperties> getRequiredVulkanExtensions();
	static bool							getPresentationSupport(const vk::Instance& instance, const vk::PhysicalDevice& device, uint32_t family);
	static std::vector<uint32_t>		getPresentationQueueFamilies(const vk::Instance& instance, const vk::PhysicalDevice& device);

	static void							setCallbacksEnabled(bool ena);
	static bool							getCallbacksEnabled();

	static const Monitor NO_MONITOR;
private:
	struct WindowGeometry {
		Math::Vec2i pos, size;
	};

	std::string                         m_name;

	GLFWwindow*                         m_window = nullptr;

	std::optional<WindowGeometry>		m_windowedState;

	Callbacks                           m_callbacks;

	void                                setupWindow();




	static std::vector<Monitor>         s_monitors;

	static std::atomic<bool>            s_exit;
	static std::atomic<bool>			s_callbacksEnabled;

	static std::thread                  s_mainThread;
	static std::mutex					s_mainThreadMutex;
	static std::condition_variable		s_mainThreadContinue;

	static std::condition_variable		s_mainThreadExecutionsComplete;
	static std::queue<std::function<void(void)>> s_mainThreadExecutions;

	static void                         mainThreadFunc();

	template<typename T, typename... Args>
	static T                            mainThreadExecute(const std::function<T(Args...)>& func, Args... args);
	static void							mainThreadContinue();

	template<typename... Args>
	static void                         callbackThreadExecute(const std::function<void(Args...)>& func, Args... args);

	static void                         cbkThreadFunc();

	static void                         setupCbks(GLFWwindow* win);
	
	static void                         monitorCbk(GLFWmonitor* mon, int evnt);
	static void                         positionCbk(GLFWwindow* win, int x, int y);
	static void                         sizeCbk(GLFWwindow* win, int x, int y);
	static void                         closeCbk(GLFWwindow* win);
	static void                         refreshCbk(GLFWwindow* win);
	static void                         focusCbk(GLFWwindow* win, int x);
	static void                         iconifyCbk(GLFWwindow* win, int x);
	static void                         maximizeCbk(GLFWwindow* win, int x);
	static void                         framebufferCbk(GLFWwindow* win, int x, int y);
	static void                         scaleCbk(GLFWwindow* win, float x, float y);


	static void                         addMonitor(GLFWmonitor* mon);
	static void                         eraseMonitor(GLFWmonitor* mon);
	static const Monitor&               getMonitor(GLFWmonitor* mon);

	static void                         init();
	static void                         terminate();
};

}