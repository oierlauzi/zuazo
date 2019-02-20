#include "Window.h"

#include <algorithm>
#include <map>
#include <utility>

#include "../../Graphics/Context.h"
#include "../../Graphics/Frame.h"
#include "../../Graphics/GL/Texture.h"
#include "../../Graphics/GL/UniqueBinding.h"
#include "../../Graphics/Shapes/Quads.h"
#include "../../Stream/SyncConsumer.h"
#include "../../Utils/ScalingModes.h"

using namespace Zuazo::Video::Consumers;

const Zuazo::Graphics::Shapes::Rectangle::RectangleData Window::WindowResources::screenVertices={
		Zuazo::Utils::Vec2f(-1.0, 	-1.0),
		Zuazo::Utils::Vec2f(+2.0, 	+2.0)
};

const std::string Window::WindowResources::vertexShader=
		#include "../../../../shaders/copy.vert"
		;

const std::string Window::WindowResources::fragmentShader=
		#include "../../../../shaders/copy.frag"
		;


/*
 * Stores the thread which continuously checks for events
 */
std::thread Window::s_eventThread;

/*
 * Stores whether the event thread needs to exit
 */
bool Window::s_exit;




std::set<std::shared_ptr<Window::Screen>> Window::Screen::s_screens;
std::mutex Window::Screen::s_cbkMutex;

/********************************
 *	INITIALIZER / TERMINATOR	*
 ********************************/

int Window::init(){
	s_exit=false;
	s_eventThread=std::thread(eventThreadFunc);

	Screen::s_screens.erase(Screen::s_screens.begin(), Screen::s_screens.end());

	//Get all the avalible sceens
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	for(int i=0; i<count; i++){
		Screen::s_screens.emplace(new Screen(monitors[i]));
	}

	glfwSetMonitorCallback(Screen::glfwMonitorCbk);

	return 0;
}

int Window::end(){
	s_exit=true;
	glfwPostEmptyEvent(); //Indicate to the event thread that it should finish
	s_eventThread.join();

	glfwSetMonitorCallback(nullptr);
	Screen::s_screens.erase(Screen::s_screens.begin(), Screen::s_screens.end());

	return 0;
}

/********************************
 *	CONSTRUCTOR / DESTRUCTOR	*
 ********************************/

Window::Window(const Utils::Resolution& res, const Utils::Rational& rate, std::string name) :
		SyncVideoConsumer(rate),
		m_name(name),
		m_resolution(res)
{
	m_glfwWindow=nullptr;
	m_exit=true;
	m_vSync=true;

	open();
}

Window::~Window(){
	close();
}

void Window::open(){
	//Ensure nothing is open
	close();

	//Check if it is fullscreen
	std::lock_guard<std::mutex> lock(Screen::s_cbkMutex);
	GLFWmonitor* monitor=nullptr;
	std::shared_ptr<Screen> screen=m_screen.lock();
	if(screen)
		monitor=screen->m_glfwMonitor;

	//Set up the GLFW window
	m_glfwWindow=glfwCreateWindow(
			m_resolution.width,							//Width
			m_resolution.height,						//Height
			m_name.c_str(),								//Name
			monitor,									//Monitor
			Graphics::Context::s_mainGlfwCtx			//Share objects with

	);

	//Set up callbacks for the window
	glfwSetWindowUserPointer(m_glfwWindow, this);
	glfwSetWindowSizeCallback(m_glfwWindow, glfwResizeCbk);
	glfwSetWindowCloseCallback(m_glfwWindow, glfwCloseCbk);

	//Set the window's context as current
	GLFWwindow* previousCtx=glfwGetCurrentContext();
	glfwMakeContextCurrent(m_glfwWindow);

	//Enable the OpenGL features that will be needed
	glEnable(GL_TEXTURE_2D);

	//Initialize the OpenGL resources
	m_glResources=std::unique_ptr<WindowResources>(new WindowResources);

	//Turn v-sync on/off
	if(m_vSync)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	//Return to the previous context
	glfwMakeContextCurrent(previousCtx);

	//Initialize the thread
	m_exit=false;
	m_drawingThread=std::thread(&Window::drawThread, this);

	//Open the consumer so updates are called
	SyncVideoConsumer::open();
}
void Window::close(){
	//Close the sync consumer -> no more updates will be requested
	SyncVideoConsumer::close();

	//Terminate the drawing thread
	m_exit=true;
	m_drawCond.notify_one();

	//Wait for termination...
	if(m_drawingThread.joinable()){
		m_drawingThread.join();
	}

	if(m_glfwWindow){
		//Release OpenGL resources
		GLFWwindow* previousCtx=glfwGetCurrentContext();
		glfwMakeContextCurrent(m_glfwWindow);
		m_glResources.reset();
		glfwMakeContextCurrent(previousCtx);

		//Destroy the window
		glfwDestroyWindow(m_glfwWindow);
		m_glfwWindow=nullptr;
	}
}

void Window::setFullScreen(const std::shared_ptr<Screen>& screen){
	std::lock_guard<std::mutex> lock(Screen::s_cbkMutex);
	if(screen->m_glfwMonitor){
		//Get windowed prameters
		std::unique_ptr<WindowedParams> windowedParams=std::unique_ptr<WindowedParams>(new WindowedParams);
		glfwGetWindowPos(m_glfwWindow, &windowedParams->pos.x, &windowedParams->pos.y);
		glfwGetWindowSize(m_glfwWindow, &windowedParams->res.x, &windowedParams->res.y);
		windowedParams->rate=getRate();

		//Set the window fullscreen
		Utils::Resolution res=screen->getRes();
		Utils::Rational rate=screen->getRate();
		glfwSetWindowMonitor(
				m_glfwWindow,							//Window
				screen->m_glfwMonitor,					//Screen
				0,										//Position: x (ignored for fullscreen)
				0,										//Position: y (ignored for fullscreen)
				res.width,								//Width
				res.height,								//Height
				static_cast<int>(rate) 					//Refresh-rate
		);

		//Update data on the window class
		m_windowed=std::move(windowedParams);
		m_screen=screen;
	}
}

void Window::setWindowed(){
	if(m_windowed){
		//Release the screen
		glfwSetWindowMonitor(
				m_glfwWindow,						//Window
				nullptr,							//Screen
				m_windowed->pos.x,					//Position: x
				m_windowed->pos.y,					//Position: y
				m_windowed->res.x,					//Width
				m_windowed->res.y,					//Height
				static_cast<int>(m_windowed->rate) 	//Refresh-rate
		);

		m_windowed.reset();
		m_screen.reset();
	}else{
		//No data is provided (should not happen)
		glfwSetWindowMonitor(
				m_glfwWindow,						//Window
				nullptr,							//Screen
				0,									//Position: x
				0,									//Position: y
				640,								//Width
				480,								//Height
				GLFW_DONT_CARE 						//Refresh-rate
		);
	}
}

/*
 * @brief sets this window as windowed if it was full-screen.
 * Otherwise it leaves it untouched
 */
void Window::setVSync(bool value) {
	if(m_vSync != value){
		std::lock_guard<std::mutex> lock(m_updateMutex);

		//vSync setting needs to be changed
		glfwMakeContextCurrent(m_glfwWindow);
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

/********************************
 *		DRAW ACTIONS			*
 ********************************/
void Window::draw() const{
	m_drawCond.notify_one();
}

void Window::update() const{
	std::shared_ptr<const Graphics::Frame> frame=SyncVideoConsumer::get();

	if(frame){
		const Graphics::GL::Texture& tex=frame->getTexture();

		GLFWwindow* previousCtx=glfwGetCurrentContext();
		glfwMakeContextCurrent(m_glfwWindow);

		//Clear
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		Graphics::GL::UniqueBinding<Graphics::GL::Texture> textureBinding(tex);
		Graphics::GL::UniqueBinding<Graphics::GL::Shader> shaderBinding(m_glResources->shader);

		m_glResources->rectangle.upload(frame->scaleFrame(
				m_resolution,
				Utils::ScalingModes::ClampVert
		));
		m_glResources->rectangle.draw();

		/*
		glColor4f(1.0, 0.0, 1.0, 1.0);
		glBegin(GL_QUADS);
			glVertex2f(-0.5, -0.5);
			glVertex2f(-0.5, +0.5);
			glVertex2f(+0.5, +0.5);
			glVertex2f(+0.5, -0.5);
		glEnd();
		*/
		glfwMakeContextCurrent(previousCtx);
	}
	draw();
}

/*
 * @brief Whenever there is a available source, it draws it on screen
 */
void Window::drawThread(){
	std::unique_lock<std::mutex> lock(m_updateMutex);
	while(!m_exit){
		m_drawCond.wait(lock); //Wait until drawing is signaled
		glfwSwapBuffers(m_glfwWindow);
	}
}

void Window::resize(const Utils::Resolution& res){
	std::unique_lock<std::mutex> lock(m_updateMutex);

	m_resolution=res;

	glfwMakeContextCurrent(m_glfwWindow);

	//Update the rendering view-port
	glViewport(0, 0, m_resolution.width, m_resolution.height);

	glfwMakeContextCurrent(NULL);
}

/********************************
 *		STATIC FUNCTIONS		*
 ********************************/

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
		window->resize(Utils::Resolution(width, height));
}

void Window::glfwCloseCbk(GLFWwindow * win){
	Window * window=(Window *)glfwGetWindowUserPointer(win);
	if(window)
		window->close();
}

void Window::Screen::glfwMonitorCbk(GLFWmonitor * mon, int event){
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	if(event == GLFW_CONNECTED){
		//There is a new screen available
		//Insert it
		s_screens.emplace(new Screen(mon));
	}else if(event == GLFW_DISCONNECTED){
		//A screen has been disconnected
		//Find it and erase it
		auto ite=s_screens.begin();
		while(ite != s_screens.end()){
			if((*ite)->m_glfwMonitor == mon){
				//Found it!
				ite=s_screens.erase(ite);
			}else{
				++ite;
			}
		}
	}
}

Window::Screen::Screen(GLFWmonitor * monitor){
	m_glfwMonitor=monitor;
}
