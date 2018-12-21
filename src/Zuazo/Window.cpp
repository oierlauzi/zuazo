#include "Window.h"

#include "Context.h"
#include <GLFW/glfw3.h>

using namespace Zuazo;

/*
 * Stores the screens that currently in use by any window
 */
std::set<GLFWmonitor *> Window::s_usedScreens;

/********************************
 *	CONSTRUCTOR / DESTRUCTOR	*
 ********************************/

Window::Window(u_int32_t width, u_int32_t height, std::string name) {
	m_ctx=glfwCreateWindow(width, height, name.c_str(), NULL, Context::s_defaultGLFWCtx);
	m_res={width, height};
	m_vSync=false;
	m_name=name;

	//Set vSync ON
	setVSync(true);
}

/*
 * @brief copy constructor
 */
Window::Window(const Window& window): Window(window.m_res.width, window.m_res.height, window.m_name) {
}

Window::~Window() {
	if(glfwGetWindowMonitor(m_ctx))
		setWindowed(); //Unuse screen

	glfwDestroyWindow(m_ctx);
}

Window::Screen::Screen(const GLFWmonitor* mon) {
	if(mon){
		GLFWmonitor* ncMon=const_cast<GLFWmonitor*>(mon);

		name=std::string(glfwGetMonitorName(ncMon)); //Query name

		//Query resolution and frame-rate
		const GLFWvidmode* vm=glfwGetVideoMode(ncMon);
		res.width=vm->width;
		res.height=vm->height;
		frameRate.num=vm->refreshRate;
		frameRate.den=1;

		//Check if it is in use
		if(s_usedScreens.find(ncMon)==s_usedScreens.end())
			isUsed=false; //Screen is not in use
		else
			isUsed=true; //Screen is being used by a Window
	}
}

/********************************
 *			SETS				*
 ********************************/

void Window::setRes(const Resolution& res) {
	setRes(res.width, res.height);
}

void Window::setRes(u_int32_t width, u_int32_t height) {
	//TODO
}

/*
 * @brief sets this window as full-screen at the given screen
 * @param screen: the target screen
 * @return true on success
 */
bool Window::setFullScreen(const Screen& screen) {
	return setFullScreen(screen.name);
}

/*
 * @brief sets this window as full-screen at the screen with the given name
 * @param screen: the target screen's name
 * @return true on success
 */
bool Window::setFullScreen(const std::string name) {
	//Query avalible screens from GLFW
	int count=0;
	GLFWmonitor** monitors = glfwGetMonitors(&count);


	//Find a screen with the same name
	GLFWmonitor* tgtMon=NULL;
	for(int i=0; i<count && !tgtMon; i++){
		std::string monName(glfwGetMonitorName(monitors[i]));
		if(monName==name)
			tgtMon=monitors[i]; //Found!
	}

	if(tgtMon){
		//Store window's position
		glfwGetWindowPos(m_ctx, &m_windowedParams.x, &m_windowedParams.y);
		glfwGetWindowSize(m_ctx, &m_windowedParams.width, &m_windowedParams.height);

		//Get screen's parameters
		int scrPosX, scrPosY;
		glfwGetMonitorPos(tgtMon, &scrPosX, &scrPosY);
		const GLFWvidmode* mode = glfwGetVideoMode(tgtMon);

		//Finally set me on fullScreen
		glfwSetWindowMonitor(m_ctx, tgtMon, scrPosX, scrPosY, mode->width, mode->height, mode->refreshRate);

		//Update size data
		m_res.width=mode->width;
		m_res.height=mode->height;

		s_usedScreens.insert(tgtMon);
		return true;
	}else
		return false;
}

void Window::setWindowed() {
	GLFWmonitor * mon=glfwGetWindowMonitor(m_ctx);
	if(mon){
		//Unset full-screen
		glfwSetWindowMonitor(
				m_ctx, NULL,
				m_windowedParams.x,
				m_windowedParams.y,
				m_windowedParams.width,
				m_windowedParams.height, 0);

		//Erase from the full-screen monitor list
		s_usedScreens.erase(mon);
	}
}

/*
 * @brief sets this window as windowed if it was full-screen.
 * Otherwise it leaves it untouched
 */
void Window::setVSync(bool value) {
	if(m_vSync != value){
		//vSync setting needs to be changed
		glfwMakeContextCurrent(m_ctx);
		if(value){
			//Turn vSyinc ON
			glfwSwapInterval(1);
			m_vSync=true;
		}else{
			//Turn vSyinc OFF
			glfwSwapInterval(0);
			m_vSync=false;
		}
	}
}
void Window::setName(std::string name){
	glfwSetWindowTitle(m_ctx, name.c_str());
}

/********************************
 *			GETS				*
 ********************************/

Resolution Window::getRes() const {
	return m_res;
}

void Window::getRes(u_int32_t* width, u_int32_t* height) const {
	*width=m_res.width;
	*height=m_res.height;
}

bool Window::isFullScreen() const {
	return glfwGetWindowMonitor(m_ctx)==NULL;
}

Window::Screen Window::getScreen() const {
	return Screen(glfwGetWindowMonitor(m_ctx));
}

bool Window::getVSync() const {
	return m_vSync;
}

std::string	Window::getName(){
	return m_name;
}

/********************************
 *		DRAW ACTIONS			*
 ********************************/

void Window::draw(const Drawtable& drawtable) {
	//TODO
}

void Window::draw(const Frame& drawtable) {
	//TODO
}

/********************************
 *		SCREEN LISTS			*
 ********************************/

/*
 * @brief returns all available screens on the system
 */
std::list<Window::Screen> Window::getScreens() {
	std::list<Window::Screen> screens;

	//Query avalible screens from GLFW
	int count=0;
	GLFWmonitor** monitors = glfwGetMonitors(&count);

	for(int i=0; i<count; i++){
		Screen scrn(monitors[i]); //Constructs the screen structure
		screens.push_back(scrn); //Add this screen to the list
	}

	return screens;
}

/*
 * @brief returns all available screens on the system that are not being
 * 			used by any window
 */
std::list<Window::Screen> Window::getAvalibleScreens() {
	std::list<Window::Screen> allScreens=getScreens(); //Get all screens
	std::list<Window::Screen> screens;

	for(Screen& scr : allScreens)
		if(!scr.isUsed)
			screens.push_back(scr); //Insert the only the ones that are not in use


	return screens;
}
