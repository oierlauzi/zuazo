#pragma once

#include <sys/types.h>
#include <list>
#include <set>
#include <string>

struct GLFWwindow;
struct GLFWmonitor;

#include "Primitives.h"

namespace Zuazo{
class Drawtable; //TODO
class Frame; //TODO

class Window{
public:
	struct Screen{
		Screen(const GLFWmonitor * mon);
		std::string 	name="";
		Resolution		res;
		Rational		frameRate;
		bool			isUsed=false;
	};

	Window(u_int32_t width=1280, u_int32_t height=720, std::string name="");
	Window(const Window& window);
	virtual ~Window();

	void			setRes(const Resolution& res);
	void			setRes(u_int32_t width, u_int32_t height);
	bool			setFullScreen(const Screen& screen);
	bool			setFullScreen(const std::string name);
	void			setWindowed();
	void			setVSync(bool value);
	void			setName(std::string name);

	Resolution		getRes() const;
	void			getRes(u_int32_t* width, u_int32_t* height) const;
	bool			isFullScreen() const;
	Screen			getScreen() const;
	bool			getVSync() const;
	std::string		getName();

	void			draw(const Drawtable& drawtable);
	void			draw(const Frame& drawtable);

	static std::list<Screen> getScreens();
	static std::list<Screen> getAvalibleScreens();
private:
	Resolution		m_res;
	std::string		m_name;
	bool			m_vSync;

	struct{
		int			x=0;
		int			y=0;
		int			width=0;
		int			height=0;
	}m_windowedParams;

	GLFWwindow* 	m_ctx; //The GLFW context

	static std::set<GLFWmonitor *> s_usedScreens;
};

}
