#include "Window.h"

#include <algorithm>

#include "../../Graphics/Context.h"
#include "../../Graphics/Frame.h"
#include "../../Graphics/GL/UniqueBinding.h"

using namespace Zuazo::Video::Consumers;

const std::array<Zuazo::Utils::Vec2f, 4> Window::WindowResources::vertices={
		Zuazo::Utils::Vec2f(-1.0, 	-1.0),
		Zuazo::Utils::Vec2f(-1.0, 	+1.0),
		Zuazo::Utils::Vec2f(+1.0, 	-1.0),
		Zuazo::Utils::Vec2f(+1.0, 	+1.0)
};

const std::array<Zuazo::Utils::Vec2f, 4> Window::WindowResources::textureCoords={
		Zuazo::Utils::Vec2f(0.0, 	0.0),
		Zuazo::Utils::Vec2f(0.0, 	1.0),
		Zuazo::Utils::Vec2f(1.0, 	0.0),
		Zuazo::Utils::Vec2f(1.0, 	1.0)
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
		m_name(name),
		m_resolution(res)
{
	//Set up the GLFW window
	m_glfwWindow=glfwCreateWindow(res.width, res.height, name.c_str(), nullptr, Graphics::Context::s_mainGlfwCtx);
	glfwSetWindowUserPointer(m_glfwWindow, this);
	glfwSetWindowSizeCallback(m_glfwWindow, glfwResizeCbk);
	glfwMakeContextCurrent(m_glfwWindow);

	//Initialize the OpenGL resources
	m_glResources=std::unique_ptr<WindowResources>(new WindowResources);

	//Turn vSync on
	glfwSwapInterval(1);
	m_vSync=true;

	 glEnable(GL_TEXTURE_2D);

	//Initialize the thread
	m_exit=false;
	m_drawingThread=std::thread(&Window::drawThread, this);

	glfwMakeContextCurrent(NULL);

	VideoOutput::setRate(rate);
}

Window::~Window() {
	VideoOutput::setInterval(0);

	//Terminate the drawing thread
	m_exit=true;
	m_drawCond.notify_one();
	m_drawingThread.join();

	//Release OpenGL resources
	glfwMakeContextCurrent(m_glfwWindow);
	m_glResources.reset();
	glfwMakeContextCurrent(NULL);

	//Destroy the window
	glfwDestroyWindow(m_glfwWindow);
}

void Window::setFullScreen(const std::shared_ptr<Screen>& screen){
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
	std::shared_ptr<const Graphics::Frame> frame=VideoConsumer::get();

	if(frame){
		const Graphics::GL::Texture& tex=frame->getTexture();

		Graphics::Context::getMainCtx().unuse();
		glfwMakeContextCurrent(m_glfwWindow);

		//Clear
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		Graphics::GL::UniqueBinding<Graphics::GL::Texture> textureBinding(tex);
		Graphics::GL::UniqueBinding<Graphics::GL::Shader> shaderBinding(m_glResources->shader);
		Graphics::GL::UniqueBinding<Graphics::GL::VertexArray<float>> vertexBinding(m_glResources->vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	/*
	glColor4f(1.0, 0.0, 1.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2f(-0.5, -0.5);
		glVertex2f(-0.5, +0.5);
		glVertex2f(+0.5, +0.5);
		glVertex2f(+0.5, -0.5);
	glEnd();
	*/


		Graphics::Context::getMainCtx().use();
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
