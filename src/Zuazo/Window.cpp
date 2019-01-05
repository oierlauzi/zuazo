#include "Window.h"

#include <GLFW/glfw3.h>
#include <map>
#include <utility>

#include "Context.h"
#include "Shader.h"

using namespace Zuazo;

const std::string Window::VERT_SHADER=
		#include "../../shaders/copy.vert"
		;
const std::string Window::FRAG_SHADER=
		#include "../../shaders/copy.frag"
		;

/*
 * Stores the screens that currently in use by any window
 */
std::set<GLFWmonitor *> Window::s_usedScreens;

/*
 * Stores the thread which continuously checks for events
 */
std::thread Window::s_eventThread;

/*
 * Stores whether the event thread needs to exit
 */
bool Window::s_exit;

/********************************
 *	INITIALIZER / TERMINATOR	*
 ********************************/

int Window::init(){
	s_exit=false;
	s_eventThread=std::thread(eventThreadFunc);
	return 0;
}

int Window::end(){
	s_exit=true;
	glfwPostEmptyEvent(); //Indicate to the event thread that it should finish
	s_eventThread.join();
	return 0;
}

/********************************
 *	CONSTRUCTOR / DESTRUCTOR	*
 ********************************/

Window::Window(u_int32_t width, u_int32_t height, std::string name) {
	m_ctx=glfwCreateWindow(width, height, name.c_str(), NULL, Context::s_mainGlfwCtx);
	glfwSetWindowUserPointer(m_ctx, this);
	glfwSetWindowSizeCallback(m_ctx, glfwResizeCbk);
	glfwMakeContextCurrent(m_ctx);

	//Create the VAO and the VBO
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(2, m_vbos);
	m_shader=Shader::create(VERT_SHADER.c_str(), FRAG_SHADER.c_str());

	//Initialize  texture coordinates and vertices on the VAO
	glBindVertexArray(m_vao);

	//The vertices
	float vertices[]={
			-1,	-1,
			-1,	1,
			1,	1,
			1, 	-1
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, 2*4*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//Texture coordinates
	float textureCorrds[]={
			0,		0,
			0,		1,
			1,		1,
			1, 		0
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, 2*4*sizeof(GLfloat), textureCorrds, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Turn vSync on
	glfwSwapInterval(1);
	m_vSync=true;

	m_res={0, 0};
	m_name=name;
	m_resizeCbk=NULL;
	m_resizeCbkObj=NULL;

	resize(width, height);

	//Initialize the thread
	m_exit=false;
	m_drawingThread=std::thread(&Window::drawThread, this);

	glfwMakeContextCurrent(NULL);
}

Window::Window(const Window& window) :
	Window(
		window.m_res.width,
		window.m_res.height,
		window.m_name
	){}

Window::~Window() {
	if(glfwGetWindowMonitor(m_ctx))
		setWindowed(); //Unuse screen

	glfwMakeContextCurrent(m_ctx);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(2, m_vbos);
	Shader::destroy(m_shader);
	glfwMakeContextCurrent(NULL);

	//Terminate the drawing thread
	m_exit=true;
	m_drawCond.notify_one();
	m_drawingThread.join();

	glfwDestroyWindow(m_ctx);
}

Window::Screen::Screen(const GLFWmonitor* monitor) {
	mon=const_cast<GLFWmonitor*>(monitor);
	if(mon){
		name=std::string(glfwGetMonitorName(mon)); //Query name

		//Query resolution and frame-rate
		const GLFWvidmode* vm=glfwGetVideoMode(mon);
		res.width=vm->width;
		res.height=vm->height;
		frameRate.num=vm->refreshRate;
		frameRate.den=1;

		//Check if it is in use
		if(s_usedScreens.find(mon)==s_usedScreens.end())
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
	if(m_res.width!=width || m_res.height!=height){
		glfwSetWindowSize(m_ctx, width, height);
	}
}

/*
 * @brief sets this window as full-screen at the given screen
 * @param screen: the target screen
 * @return true on success
 */
void Window::setFullScreen(const Screen& screen) {
	if(screen.mon){
		//Set it in windowed mode to reset any previous fullscreen
		setWindowed();

		//Store window's position
		glfwGetWindowPos(m_ctx, &m_windowedParams.x, &m_windowedParams.y);
		glfwGetWindowSize(m_ctx, &m_windowedParams.width, &m_windowedParams.height);

		//Get screen's parameters
		int scrPosX, scrPosY;
		glfwGetMonitorPos(screen.mon, &scrPosX, &scrPosY);
		const GLFWvidmode* mode = glfwGetVideoMode(screen.mon);

		//Finally set me on fullScreen
		glfwSetWindowMonitor(m_ctx, screen.mon, scrPosX, scrPosY, mode->width, mode->height, mode->refreshRate);

		s_usedScreens.insert(screen.mon);
	}
}

/*
 * @brief sets this window as full-screen at the screen with the given name
 * @param screen: the target screen's name
 * @return true on success
 */
bool Window::setFullScreen(const std::string name) {
	//Get all the screens
	std::list<Screen> screens=getScreens();

	//Find the requested screen
	Screen* screen=NULL;
	for(Screen& scr : screens)
		if(scr.name==name && !scr.isUsed)
			screen=&scr;	//Found!


	if(screen){
		setFullScreen(*screen);
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
		std::unique_lock<std::mutex> lock(m_mutex);

		//vSync setting needs to be changed
		glfwMakeContextCurrent(m_ctx);
		if(value){
			//Turn vSync ON
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

/*
 * @brief Sets the callback which will be called whenever the window gets resized
 * @param resizeCbk: A pointer to a function with the following prototype: void f(u_int32_t, u_int32_t)
 */
void Window::setResizeCbk(void(*resizeCbk)(u_int32_t width, u_int32_t height)){
	m_resizeCbk=resizeCbk;
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

/*
 * @brief Draws the given surface on screen
 * @param surface: The surface which is going to be drawn on screen
 */
void Window::show(const Surface& surface) {
	std::unique_lock<std::mutex> lock(m_mutex);

	//Evaluate whether resizing needs to be done
	if(surface.m_res != m_res){
		glfwSetWindowSize(m_ctx, surface.m_res.width, surface.m_res.height);
		m_resizeCond.wait(lock); //Wait until resizing has finished
	}

	glfwMakeContextCurrent(m_ctx);

	//Clear
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, surface.getTexture());
	glBindVertexArray(m_vao);

	glUseProgram(m_shader);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUseProgram(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	glfwMakeContextCurrent(NULL);
	m_drawCond.notify_one();
}

/*
 * @brief Draws the given image on screen
 * @param image: The image which is going to be drawn on screen
 */
void Window::show(const Image& img){
	std::unique_lock<std::mutex> lock(m_mutex);

	//Evaluate whether resizing needs to be done
	if(img.m_res != m_res){
		glfwSetWindowSize(m_ctx, img.m_res.width, img.m_res.height);
		m_resizeCond.wait(lock); //Wait until resizing has finished
	}

	glfwMakeContextCurrent(m_ctx);

	//Clear
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, img.m_pbo);
	glDrawPixels(	img.m_res.width, img.m_res.height,
					GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	glfwMakeContextCurrent(NULL);

	m_drawCond.notify_one();
}

/*
 * @brief Whenever there is a available source, it draws it on screen
 */
void Window::drawThread(){
	std::unique_lock<std::mutex> lock(m_mutex);
	while(!m_exit){
		m_drawCond.wait(lock); //Wait until drawing is signaled
		glfwSwapBuffers(m_ctx);
	}
}

void Window::resize(u_int32_t width, u_int32_t height){
	std::unique_lock<std::mutex> lock(m_mutex);

	m_res={width, height};

	glfwMakeContextCurrent(m_ctx);

	//Update the rendering view-port
	glViewport(0, 0, m_res.width, m_res.height);

	glfwMakeContextCurrent(NULL);

	//If the window has a resize callback, call it
	if(m_resizeCbk){
		if(m_resizeCbkObj)
			printf("Object's callback todo");//TODO
		else
			m_resizeCbk(width, height);
	}

	m_resizeCond.notify_all();
}

/********************************
 *		STATIC FUNCTIONS		*
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

/*
 * @brief Continuously checks for events in GLFW
 */
void Window::eventThreadFunc(){
	while(!s_exit){
		glfwWaitEvents();
	}
}

/*
 * @brief function which gets called by GLFW when a window changes it's resolution
 */
void Window::glfwResizeCbk(GLFWwindow * win, int width, int height){
	Window * window=(Window *)glfwGetWindowUserPointer(win);
	if(window)
		window->resize(width, height);
}
