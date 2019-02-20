#pragma once

#include <glm/detail/type_vec2.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>

#include "../../Graphics/GL/Shader.h"
#include "../../Graphics/Shapes/Rectangle.h"
#include "../../Utils/Rational.h"
#include "../../Utils/Resolution.h"
#include "../../Utils/Vector.h"
#include "../Video.h"

namespace Zuazo::Video::Consumers{

class Window :
	public SyncVideoConsumer
{
public:
	class Screen{
		friend Window;
	public:
		Screen()=delete;
		Screen(const Screen& other)=delete;
		~Screen()=default;

		std::string									getName() const;
		Utils::Resolution 							getRes() const;
		Utils::Vec2i								getSize() const;
		Utils::Vec2i								getPos() const;
		Utils::Rational								getRate() const;
		bool										isAvalible() const;

		static std::set<std::shared_ptr<Screen>> 	getScreens();
	private:
		Screen(GLFWmonitor * monitor);

		GLFWmonitor	*								m_glfwMonitor;

		static std::set<std::shared_ptr<Screen>>	s_screens;
		static std::mutex							s_cbkMutex;

		static void 								glfwMonitorCbk(GLFWmonitor * mon, int event);
	};

	static int init();
	static int end();

	Window()=delete;
	Window(const Utils::Resolution& res=Utils::Resolution(640, 480), const Utils::Rational& rate=60, std::string name="");
	Window(const Window& win)=delete;
	virtual ~Window();

	void										open() override;
	void										close() override;

	void										setRes(const Utils::Resolution& res);
	void										setFullScreen(const std::shared_ptr<Screen>& screen);
	void										setWindowed();
	void										setVSync(bool value);
	void										setName(const std::string& name);

	Utils::Resolution							getRes() const;
	bool										isFullScreen() const;
	std::shared_ptr<Screen>						getScreen() const;
	bool										getVSync() const;
	std::string									getName() const;
protected:
	void										update() const override;
private:
	struct WindowedParams{
		Utils::Vec2<int> 							res;
		Utils::Vec2<int>							pos;
		Utils::Rational								rate;
	};

	struct WindowResources{
		WindowResources() :
			shader(vertexShader, fragmentShader),
			rectangle(screenVertices)
		{
		}
		WindowResources(const WindowResources& other)=delete;
		~WindowResources()=default;

		Graphics::GL::Shader						shader;
		Graphics::Shapes::Rectangle					rectangle;
	private:
		static const std::string					vertexShader;
		static const std::string					fragmentShader;
		static const Graphics::Shapes::Rectangle::RectangleData screenVertices;
	};

 	//The GLFW window
	GLFWwindow* 								m_glfwWindow;

	//Window Data
	std::string									m_name;
	Utils::Resolution 							m_resolution;
	bool										m_vSync;
	std::weak_ptr<Screen>						m_screen;
	std::unique_ptr<WindowedParams> 			m_windowed;

	//Drawing thread options
	std::thread									m_drawingThread;
	mutable std::condition_variable 			m_drawCond;
	bool										m_exit;

	std::unique_ptr<WindowResources> 			m_glResources;

	void										draw() const;
	void										drawThread();
	void										resize(const Utils::Resolution& res);

	//Event thread stuff
	static std::thread							s_eventThread;
	static bool									s_exit;

	static void									eventThreadFunc();
	static void									glfwResizeCbk(GLFWwindow * win, int width, int height);
	static void									glfwCloseCbk(GLFWwindow * win);
};

/*
 * SCREEN'S INLINE METHOD DEFINITIONS
 */

inline std::string Window::Screen::getName() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	if(m_glfwMonitor){
		return std::string(glfwGetMonitorName(m_glfwMonitor));
	}else
		return "";
}

inline Utils::Resolution Window::Screen::getRes() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	if(m_glfwMonitor){
		const GLFWvidmode* videoMode=glfwGetVideoMode(m_glfwMonitor);
		return Utils::Resolution(videoMode->width, videoMode->height);
	}else
		return Utils::Resolution();
}

inline Utils::Vec2<int> Window::Screen::getSize() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	if(m_glfwMonitor){
		int width, height;
		glfwGetMonitorPhysicalSize(m_glfwMonitor, &width, &height);
		return Utils::Vec2<int>(width, height);
	}else
		return Utils::Vec2<int>(-1, -1);
}

inline Utils::Vec2<int> Window::Screen::getPos() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	if(m_glfwMonitor){
		int x, y;
		glfwGetMonitorPos(m_glfwMonitor, &x, &y);
		return Utils::Vec2<int>(x, y);
	}else
		return Utils::Vec2<int>(-1, -1);
}

inline Utils::Rational Window::Screen::getRate() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	if(m_glfwMonitor){
		const GLFWvidmode* videoMode=glfwGetVideoMode(m_glfwMonitor);
		return Utils::Rational(videoMode->refreshRate, 1);
	}else
		return Utils::Rational();
}

inline bool Window::Screen::isAvalible() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	return m_glfwMonitor != nullptr;
}

inline std::set<std::shared_ptr<Window::Screen>> Window::Screen::getScreens(){
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	return s_screens;
}

/*
 * WINDOW INLINE METHOD DEFINITIONS
 */

/********************************
 *			SETS				*
 ********************************/

inline void Window::setRes(const Utils::Resolution& res) {
	if(m_resolution != res){
		glfwSetWindowSize(m_glfwWindow, res.width, res.height); //Callback should update the actual data
	}
}

inline void Window::setName(const std::string& name){
	glfwSetWindowTitle(m_glfwWindow, name.c_str());
}

/********************************
 *			GETS				*
 ********************************/

inline Zuazo::Utils::Resolution Window::getRes() const {
	return m_resolution;
}

inline bool Window::getVSync() const {
	return m_vSync;
}

inline std::string	Window::getName() const{
	return m_name;
}

inline bool Window::isFullScreen() const{
	return !m_screen.use_count();
}

inline std::shared_ptr<Window::Screen> Window::getScreen() const{
	return m_screen.lock();
}

}
