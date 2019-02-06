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

#include "Utils/Resolution.h"
#include "Utils/Rational.h"
#include "Graphics/GL/Shader.h"

struct GLFWwindow;
struct GLFWmonitor;

namespace Zuazo{

class Window{
public:
	struct Screen{
		friend Window;
		std::string 		name;
		Utils::Resolution	res;
		Utils::Rational		frameRate;
		bool				isUsed;
	private:
		Screen(const GLFWmonitor * monitor);
		GLFWmonitor	*	mon=NULL;

	};

	static int init();
	static int end();

	Window(u_int32_t width=1280, u_int32_t height=720, std::string name="");
	Window(const Window& window);
	virtual ~Window();

	void						setRes(const Utils::Resolution& res);
	void						setRes(u_int32_t width, u_int32_t height);
	void						setFullScreen(const Screen& screen);
	bool						setFullScreen(const std::string name);
	void						setWindowed();
	void						setVSync(bool value);
	void						setName(std::string name);
	void						setResizeCbk(void(*resizeCbk)(u_int32_t width, u_int32_t height));

	Utils::Resolution			getRes() const;
	void						getRes(u_int32_t* width, u_int32_t* height) const;
	bool						isFullScreen() const;
	Screen						getScreen() const;
	bool						getVSync() const;
	std::string					getName();

	/*void			show(const Surface& surface);
	void			show(const Image& img);*/

	static std::list<Screen>	getScreens();
	static std::list<Screen> 	getAvalibleScreens();
private:
	//Window Data
	Utils::Resolution			m_res;
	std::string					m_name;
	bool						m_vSync;

	//Drawing thread options
	std::thread					m_drawingThread;
	std::mutex					m_mutex;
	std::condition_variable 	m_drawCond;
	bool						m_exit;

	//Window's state before it was full-screen (if applicable)
	struct{
		int			x=0;
		int			y=0;
		int			width=0;
		int			height=0;
	}m_windowedParams;

	void			(*m_resizeCbk)(u_int32_t width, u_int32_t height); 	//Resize callback
	void *			m_resizeCbkObj;

	GLFWwindow* 	m_ctx; 	//The GLFW window

	//VBO, VAO and Shader for the surface drawing
	GLuint			m_vbos[2];
	GLuint			m_vao;
	std::unique_ptr<Graphics::GL::Shader> m_shader;

	std::condition_variable m_resizeCond;

	void			drawThread();
	void			resize(u_int32_t width, u_int32_t height);

	static const std::string VERT_SHADER;
	static const std::string FRAG_SHADER;

	static std::set<GLFWmonitor *> s_usedScreens;

	//Event thread stuff
	static std::thread	s_eventThread;
	static bool		s_exit;

	static void		eventThreadFunc();
	static void		glfwResizeCbk(GLFWwindow * win, int width, int height);
};

}
