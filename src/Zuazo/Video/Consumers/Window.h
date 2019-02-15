#pragma once

#include <GL/glew.h>
#include <sys/types.h>
#include <condition_variable>
#include <list>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <memory>

#include "../../Utils/Resolution.h"
#include "../../Utils/Rational.h"
#include "../../Utils/VideoMode.h"
#include "../../Utils/Vector.h"
#include "../Video/Video.h"
#include "../../Timing/PeriodicUpdate.h"
#include "Graphics/GL/Shader.h"

struct GLFWwindow;
struct GLFWmonitor;

namespace Zuazo::Video::Consumers{

class Screen;

class Window :
	public VideoConsumer,
	public Timing::PeriodicUpdate
{
public:
	static int init();
	static int end();

	Window();
	Window(const Utils::VideoMode& mode, std::string name="");
	Window(const Window& win);
	virtual ~Window();

	void						setRes(const Utils::Resolution& res);
	void						setFullScreen(const Screen& screen);
	void						setWindowed();
	void						setVSync(bool value);
	void						setName(std::string name);

	Utils::Resolution			getRes() const;
	bool						isFullScreen() const;
	std::shared_ptr<Screen>		getScreen() const;
	bool						getVSync() const;
	std::string					getName() const;
private:
	struct WindowedParams{
		Utils::Resolution 			res;
		Utils::Vec2<int>			pos;
	};

 	//The GLFW window
	GLFWwindow* 				m_ctx;

	//Window Data
	std::string					m_name;
	Utils::Resolution 			m_resolution;
	bool						m_vSync;
	std::weak_ptr<Screen>		m_screen;
	std::unique_ptr<WindowedParams> m_windowed;

	//Drawing thread options
	std::thread					m_drawingThread;
	std::mutex					m_mutex;
	std::condition_variable 	m_drawCond;
	bool						m_exit;

	//VBO, VAO and Shader for the surface drawing
	Graphics::GL::Shader 		m_shader;

	std::condition_variable 	m_resizeCond;

	void						drawThread();
	void						resize(u_int32_t width, u_int32_t height);

	//Event thread stuff
	static std::thread			s_eventThread;
	static bool					s_exit;

	static void					eventThreadFunc();
	static void					glfwResizeCbk(GLFWwindow * win, int width, int height);
};


class Screen{
public:
	Screen()=delete;
	Screen(const Screen& other)=delete;
	~Screen()=default;

	std::string						getName() const;
	Utils::Resolution 				getRes() const;
	Utils::Vec2<int>				getSize() const;
	Utils::Vec2<int>				getPos() const;
	Utils::Rational					getRate() const;
	bool							isAvalible() const;

	static const std::set<std::shared_ptr<Screen>>& getScreens();
private:
	Screen(const GLFWmonitor * monitor);

	GLFWmonitor	*					m_monitor;

	static const std::set<std::shared_ptr<Screen>> 	s_screens;
	static std::mutex				s_cbkMutex;

	static void 					monitorCbk(GLFWmonitor * mon, int event);

};



/*
 * INLINE METHOD DEFINITIONS
 */

inline std::string Screen::getName() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	return std::string(glfwGetMonitorName(m_monitor));
}

inline Utils::Resolution Screen::getRes() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	const GLFWvidmode* videoMode=glfwGetVideoMode(m_monitor);
	return Utils::Resolution(videoMode->width, videoMode->height);
}

inline Utils::Vec2<int> Screen::getSize() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	int width, height;
	glfwGetMonitorPhysicalSize(m_monitor, &width, &height);
	return Utils::Vec2<int>(width, height);
}

inline Utils::Vec2<int> Screen::getPos() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	int x, y;
	glfwGetMonitorPos(m_monitor, &x, &y);
	return Utils::Vec2<int>(x, y);
}

inline Utils::Rational Screen::getRate() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	const GLFWvidmode* videoMode=glfwGetVideoMode(m_monitor);
	return Utils::Rational(videoMode->refreshRate, 1);
}

}
