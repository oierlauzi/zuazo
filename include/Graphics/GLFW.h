#pragma once

#include "Vulkan.h"
#include "../Math/Vector.h"
#include "../Resolution.h"
#include "../Timing/Chrono.h"

#include <atomic>
#include <cstddef>
#include <memory>
#include <string_view>

struct GLFWmonitor;
struct GLFWwindow;

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

	static void									setCallbacksEnabled(bool ena);
	static bool									getCallbacksEnabled();

	static const Monitor NO_MONITOR;

private:
	using MonitorHandle = GLFWmonitor*;
	using WindowHandle = GLFWwindow*;

	struct Callbacks;

	struct Impl;
	static std::unique_ptr<Impl>		s_impl;

	static std::atomic<size_t>			s_instanceCount;

	static MonitorCallback				s_monitorCbk;
	
};


class GLFW::Monitor {
	friend GLFW::Impl;
	friend GLFW::Callbacks;
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

};



class GLFW::Window {
	friend GLFW::Impl;
	friend GLFW::Callbacks;
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
	Window(	Math::Vec2i size, 
			std::string_view name );
	Window(const Window& other) = delete;
	Window(Window&& other);
	~Window();

	Window& operator=(const Window& other) = delete;
	Window& operator=(Window&& other);

	operator bool() const;

	vk::UniqueSurfaceKHR 		getSurface(const Vulkan& vulkan) const;

	void						setName(std::string_view name);

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

	void						setDecorated(bool deco);
	bool						getDecorated() const;

	void						setResizeable(bool resizeable);
	bool						getResizeable() const;


private:
	struct Geometry {
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
	Geometry 						m_windowedState = {};

	WindowHandle 					m_window = nullptr;

};

}