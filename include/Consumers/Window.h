#pragma once

#include "../glad/glad.h"
#include "../Graphics/GL/Shader.h"
#include "../Graphics/ShaderUniform.h"
#include "../Graphics/VertexArray.h"
#include "../Utils/Rational.h"
#include "../Utils/Resolution.h"
#include "../Utils/Vector.h"
#include "../Video/VideoStream.h"
#include "../Video/VideoConsumerBase.h"
#include "../Timing.h"
#include "../ZuazoBase.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>

namespace Zuazo::Consumers{

class Window :
	public Video::TVideoConsumerBase<Video::VideoConsumerPad>,
	public ZuazoBase,
	private Timing::PeriodicUpdate
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
	static int terminate();

	Window()=delete;
	Window(const Utils::VideoMode& videoMode, std::string name="Zuazo");
	Window(const Window& win)=delete;
	virtual ~Window();

	void										open() override;
	void										close() override;

	SUPPORTS_GETTING_RESOLUTION
	SUPPORTS_SETTING_RESOLUTION
	SUPPORTS_ANY_RESOLUTION
	void										setResolution(const Utils::Resolution& res) override;

	SUPPORTS_GETTING_SCALINGMODE
	SUPPORTS_SETTING_SCALINGMODE
	void										setScalingMode(Utils::ScalingMode mode) override;

	SUPPORTS_GETTING_SCALINGFILTER
	SUPPORTS_SETTING_SCALINGFILTER
	void										setScalingFilter(Utils::ScalingFilter fltr) override;

	SUPPORTS_GETTING_FRAMERATE
	SUPPORTS_SETTING_FRAMERATE
	SUPPORTS_ANY_FRAMERATE
	void										setFramerate(const Utils::Rational& rate) override;

	void										setFullScreen(const std::shared_ptr<Screen>& screen);
	void										setWindowed();
	void										setVSync(bool value);
	void										setTitle(const std::string& title);

	bool										isFullScreen() const;
	const std::weak_ptr<Screen>&				getScreen() const;
	bool										getVSync() const;
	const std::string&							getTitle() const;

	void										update() const override;
private:
	struct WindowedParams{
		Utils::Vec2<int> 							res;
		Utils::Vec2<int>							pos;
		Utils::Rational								rate;
	};

	struct WindowResources{
		WindowResources();
		WindowResources(const WindowResources& other)=delete;
		~WindowResources()=default;

		Graphics::GL::Program						shader;
		Graphics::VertexArray						vertices;
		Graphics::ShaderUniform						uniforms;

		const GLint									vertAttribIdx;
		const GLint									texCoordAttribIdx;
	private:
		static const std::string					vertexShader;
		static const std::string					fragmentShader;
		static const std::array<Zuazo::Utils::Vec2f, 4> screenVertices;
	};

 	//The GLFW window
	GLFWwindow* 								m_glfwWindow;

	//Window Data
	std::string									m_title;
	bool										m_vSync;
	std::weak_ptr<Screen>						m_screen;
	std::unique_ptr<WindowedParams> 			m_windowed;

	//Drawing thread options
	std::thread									m_drawingThread;
	mutable	std::mutex							m_mutex;
	mutable std::condition_variable 			m_drawCond;
	bool										m_exit;

	std::unique_ptr<WindowResources> 			m_resources;

	mutable Utils::Resolution					m_lastFrameRes;
	mutable bool								m_forceDraw;

	void										draw() const;
	void										drawThread();
	void										resize(const Utils::Resolution& res);

	//Event thread stuff
	static std::thread							s_eventThread;
	static bool									s_exit;

	static const Utils::VideoMode				s_defaultWindow;

	static void									eventThreadFunc();
	static void									glfwResizeCbk(GLFWwindow * win, int width, int height);
	static void									glfwCloseCbk(GLFWwindow * win);
};

/*
 * WINDOW INLINE METHOD DEFINITIONS
 */

/********************************
 *			SETS				*
 ********************************/

inline void Window::setResolution(const Utils::Resolution& res) {
	if(m_videoMode.res != res){
		glfwSetWindowSize(m_glfwWindow, res.width, res.height); //Callback should update the actual data
	}
}

inline void	Window::setScalingMode(Utils::ScalingMode mode){
	m_scalingMode=mode;
	m_forceDraw=true;
}

inline void Window::setTitle(const std::string& title){
	glfwSetWindowTitle(m_glfwWindow, title.c_str());
	m_title=title;
}

/********************************
 *			GETS				*
 ********************************/

inline bool Window::getVSync() const {
	return m_vSync;
}

inline const std::string&	Window::getTitle() const{
	return m_title;
}

inline bool Window::isFullScreen() const{
	return !m_screen.use_count();
}

inline const std::weak_ptr<Window::Screen>& Window::getScreen() const{
	return m_screen;
}

}
