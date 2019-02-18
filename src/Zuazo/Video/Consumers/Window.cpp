#include "Window.h"

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

Window::Window(const Utils::Resolution& res, const Utils::Rational& rate, std::string name) :
		m_name(name),
		m_resolution(res)
{
	//Set up the GLFW window
	m_glfwWindow=glfwCreateWindow(res.width, res.height, name.c_str(), NULL, Graphics::Context::s_mainGlfwCtx);
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

/********************************
 *		DRAW ACTIONS			*
 ********************************/
void Window::draw() const{
	m_drawCond.notify_one();
}

void Window::update() const{
	Graphics::Context::mainCtx->unuse();
	glfwMakeContextCurrent(m_glfwWindow);

	//Clear
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	std::shared_ptr<const Graphics::Frame> frame=VideoConsumer::get();
	if(frame){
		Graphics::GL::UniqueBinding<Graphics::Frame> frameBinding(*frame);
		Graphics::GL::UniqueBinding<Graphics::GL::Shader> shaderBinding(m_glResources->shader);
		Graphics::GL::UniqueBinding<Graphics::GL::VertexArray<float>> vertexBinding(m_glResources->vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glfwMakeContextCurrent(nullptr);
	Graphics::Context::mainCtx->use();
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
