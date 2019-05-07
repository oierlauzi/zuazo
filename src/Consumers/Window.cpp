#include <Consumers/Window.h>

#include <Graphics/Context.h>
#include <Graphics/Frame.h>
#include <Graphics/GL/Texture2D.h>
#include <Graphics/GL/UniqueBinding.h>

#include <glm/detail/type_vec2.hpp>
#include <algorithm>
#include <map>
#include <array>
#include <utility>

using namespace Zuazo::Consumers;

const std::array<Zuazo::Utils::Vec2f, 4> Window::WindowResources::screenVertices{
		Zuazo::Utils::Vec2f(-1.0, 	-1.0), 	//Bottom left
		Zuazo::Utils::Vec2f(-1.0,	+1.0),	//Top left
		Zuazo::Utils::Vec2f(+1.0,	-1.0),	//Bottom right
		Zuazo::Utils::Vec2f(+1.0,	+1.0),	//Top right
};

const std::string Window::WindowResources::vertexShader(
		#include "../../data/shaders/window.vert"
);

const std::string Window::WindowResources::fragmentShader(
		#include "../../data/shaders/window.frag"
);


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

int Window::terminate(){
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

Window::Window(const Utils::VideoMode& videoMode, std::string name) :
		Timing::PeriodicUpdate(UpdatePriorities::CONSUMER),
		m_title(name)
{
	m_videoMode=videoMode;

	m_glfwWindow=nullptr;
	m_exit=true;
	m_vSync=true;

	open();
}

Window::~Window(){
	close();
}

void Window::open(){
	if(m_videoMode.res && m_videoMode.frameRate){
		//Check if it is fullscreen
		std::lock_guard<std::mutex> lock(Screen::s_cbkMutex);
		GLFWmonitor* monitor=nullptr;
		std::shared_ptr<Screen> screen=m_screen.lock();
		if(screen)
			monitor=screen->m_glfwMonitor;

		//Set up the GLFW window
		m_glfwWindow=glfwCreateWindow(
				m_videoMode.res.width,						//Width
				m_videoMode.res.height,						//Height
				m_title.c_str(),							//Name
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
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

		//Initialize the OpenGL resources
		m_resources=std::unique_ptr<WindowResources>(new WindowResources);
		m_resources->uniforms.setParam<int>(static_cast<int>(m_scalingFilter), 0);

		//Return to the previous context
		glfwMakeContextCurrent(previousCtx);

		//Turn v-sync on/off
		if(m_vSync)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		//Initialize the thread
		m_exit=false;
		m_drawingThread=std::thread(&Window::drawThread, this);

		m_forceDraw=true;
		m_lastFrameRes=Utils::Resolution();

		//Open the consumer so updates are called
		Timing::PeriodicUpdate::setRate(m_videoMode.frameRate);
		Timing::PeriodicUpdate::enable();

		ZuazoBase::open();
	}
}
void Window::close(){
	//Close the sync consumer -> no more updates will be requested
	Timing::PeriodicUpdate::disable();

	m_videoConsumerPad.reset();

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
		m_resources.reset();
		glfwMakeContextCurrent(previousCtx);

		//Destroy the window
		glfwDestroyWindow(m_glfwWindow);
		m_glfwWindow=nullptr;
	}
	ZuazoBase::close();
}

void Window::setScalingFilter(Utils::ScalingFilter fltr){
	m_scalingFilter=fltr;
	if(m_resources){
		m_resources->uniforms.setParam<int>(static_cast<int>(m_scalingFilter), 0);
	}
}

void Window::setFullScreen(const std::shared_ptr<Screen>& screen){
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


void Window::setFramerate(const Utils::Rational& rate){
	std::lock_guard<std::mutex> lock(m_videoModeMutex);
	m_videoMode.frameRate=rate;

	if(isOpen())
		Timing::PeriodicUpdate::setRate(m_videoMode.frameRate);
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
	std::lock_guard<std::mutex> lockUpdate(m_mutex);
	if(m_videoConsumerPad.hasChanged() || m_forceDraw){
		std::shared_ptr<const Graphics::Frame> frame=m_videoConsumerPad.get();
		const Graphics::GL::Texture2D* tex=nullptr;

		if(frame){
			tex=&(frame->getTexture());
		}

		GLFWwindow* previousCtx=glfwGetCurrentContext();
		glfwMakeContextCurrent(m_glfwWindow);

		//Clear
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		if(tex){
			const Utils::Resolution& srcRes=frame->getAttributes().res;
			if((m_lastFrameRes != srcRes || m_forceDraw) && srcRes){
				//Vertex buffer needs to be updated

				const Utils::Resolution& dstRes=m_videoMode.res;
				float sX, sY, s;

				//Calculate the scale
				sX=(float)dstRes.width / srcRes.width;
				sY=(float)dstRes.height / srcRes.height;


				//Evaluate drawing area's dimensions
				switch(m_scalingMode){
				case Utils::ScalingMode::Boxed:
					//Scale with the minimum factor
					s=std::min(sX, sY);
					sX=s;
					sY=s;
					break;
				case Utils::ScalingMode::Cropped:
					//Scale with the maximum factor
					s=std::max(sX, sY);
					sX=s;
					sY=s;
					break;
				case Utils::ScalingMode::Stretched:
					//Scale independently.
					//Neither sX nor sY needs to be modified
					break;
				case Utils::ScalingMode::ClampVert:
					//Scale according to sY
					sX=sY;
					break;
				case Utils::ScalingMode::ClampHor:
					//Scale according to sX
					sY=sX;
					break;
				default:
					//This should not happen
					//Don't show anything
					sX=0;
					sY=0;
					break;
				}

				float diffX = (float)dstRes.width / (2 * srcRes.width * sX);
				float diffY = (float)dstRes.height / (2 * srcRes.height * sY);

				std::array<Zuazo::Utils::Vec2f, 4> vertices{  //Invert Y coordinates
						Zuazo::Utils::Vec2f(0.5f - diffX, 	0.5f + diffY),
						Zuazo::Utils::Vec2f(0.5f - diffX, 	0.5f - diffY),
						Zuazo::Utils::Vec2f(0.5f + diffX, 	0.5f + diffY),
						Zuazo::Utils::Vec2f(0.5f + diffX, 	0.5f - diffY),
				};

				m_resources->vertices.uploadVertices(m_resources->texCoordAttribIdx, vertices);
				m_lastFrameRes=srcRes;
			}

			//Finaly draw everithing
			Graphics::GL::UniqueBinding<Graphics::VertexArray> vaoBind(m_resources->vertices);
			Graphics::GL::UniqueBinding<Graphics::GL::Program> shaderBind(m_resources->shader);
			Graphics::GL::UniqueBinding<Graphics::GL::Texture2D> texBind(*tex);
			m_resources->uniforms.bind();
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		glfwMakeContextCurrent(previousCtx);
		draw();

		m_forceDraw=false;
	}
}

/*
 * @brief Whenever there is a available source, it draws it on screen
 */
void Window::drawThread(){
	std::unique_lock<std::mutex> lock(m_mutex);
	while(!m_exit){
		m_drawCond.wait(lock); //Wait until drawing is signaled
		glfwSwapBuffers(m_glfwWindow);
	}
}

void Window::resize(const Utils::Resolution& res){
	std::lock_guard<std::mutex> lockUpdate(m_mutex);

	m_videoMode.res=res;
	m_forceDraw=true;

	glfwMakeContextCurrent(m_glfwWindow);

	//Update the rendering view-port
	glViewport(0, 0, m_videoMode.res.width, m_videoMode.res.height);

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

Window::WindowResources::WindowResources() :
	shader(vertexShader, fragmentShader),
	uniforms(0, shader.getUniformBlockSize("scalingFilterBlock")),
	vertAttribIdx(shader.getAttributeLoc("in_vertex")),
	texCoordAttribIdx(shader.getAttributeLoc("in_uv"))
{	
	vertices.enableAttribute(vertAttribIdx, screenVertices);
	vertices.enableAttribute<float, 2>(texCoordAttribIdx, 4);
	shader.setUniformBlockBinding("scalingFilterBlock", 0);
}


std::string Window::Screen::getName() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	if(m_glfwMonitor){
		return std::string(glfwGetMonitorName(m_glfwMonitor));
	}else
		return "";
}

Zuazo::Utils::Resolution Window::Screen::getRes() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	if(m_glfwMonitor){
		const GLFWvidmode* videoMode=glfwGetVideoMode(m_glfwMonitor);
		return Utils::Resolution(videoMode->width, videoMode->height);
	}else
		return Utils::Resolution();
}

Zuazo::Utils::Vec2<int> Window::Screen::getSize() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	if(m_glfwMonitor){
		int width, height;
		glfwGetMonitorPhysicalSize(m_glfwMonitor, &width, &height);
		return Utils::Vec2<int>(width, height);
	}else
		return Utils::Vec2<int>(-1, -1);
}

Zuazo::Utils::Vec2<int> Window::Screen::getPos() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	if(m_glfwMonitor){
		int x, y;
		glfwGetMonitorPos(m_glfwMonitor, &x, &y);
		return Utils::Vec2<int>(x, y);
	}else
		return Utils::Vec2<int>(-1, -1);
}

Zuazo::Utils::Rational Window::Screen::getRate() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	if(m_glfwMonitor){
		const GLFWvidmode* videoMode=glfwGetVideoMode(m_glfwMonitor);
		return Utils::Rational(videoMode->refreshRate, 1);
	}else
		return Utils::Rational();
}

bool Window::Screen::isAvalible() const{
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	return m_glfwMonitor != nullptr;
}

std::set<std::shared_ptr<Window::Screen>> Window::Screen::getScreens(){
	std::lock_guard<std::mutex> lock(s_cbkMutex);

	return s_screens;
}
