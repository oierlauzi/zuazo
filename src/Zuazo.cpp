#include <Zuazo.h>

#include <Consumers/Window.h>
#include <Graphics/Context.h>
#include <Timing/Timings.h>
#include <Sources/ImageMagick.h>
#include <Sources/SVG.h>
#include <Sources/V4L2.h>
#include <Sources/FFmpeg.h>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Magick++/Functions.h>
#include <Video/VideoSourceBase.h>

namespace Zuazo{

std::unique_ptr<Timing::Timings> timings;

/***************************************************
 *  INITIALIZATION / QUIT FUNCTIONS                *
 ***************************************************/

/**
	@brief initializes Zuazo
	@return Error generated initializing Error::NONE for all OK
 **/
Errors init(){
    int err;
    //Init GLFW
    err=glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if(err!=GLFW_TRUE)
        return Errors::GLFW_INIT;

    //Initialize context handling
    err=Graphics::Context::init();
    if(err)
    	return Errors::CONTEX_INIT;

    Graphics::UniqueContext ctx(Graphics::Context::getMainCtx());

    //Init GLAD
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        return Errors::GLAD_INIT;

    //Initialize window class
    err=Consumers::Window::init();
    if(err)
    	return Errors::WINDOW_INIT;

    Magick::InitializeMagick(nullptr);

    //Initialize timing
    timings=std::unique_ptr<Timing::Timings>(new Timing::Timings);

//TODO
    /*err=Shapes::Base::init();
    if(err!=OK)
        return Error::SHAPES_INIT;


    err=Text::init();
    if(err!=OK)
        return Error::TEXT_INIT;*/

    return Errors::NONE;
}

/**
	@brief ends Zuazo. No more zuazo functions can be called
	@return Error generated destroying Error::NONE for all OK
 **/
Errors end(){
	Consumers::Window::end();
	timings.reset();
	Graphics::Context::end();
    return Errors::NONE; //TODO
}

std::unique_ptr<Video::VideoSourceBase> videoSourceFromFile(const std::string& file){
	const std::string fileExtension=file.substr(file.find_last_of(".") + 1);

	//Check V4L2 compatibility
	if(file.find("/dev/video") == 0)
		return std::unique_ptr<Video::VideoSourceBase>(new Sources::V4L2(file));

	//Check SVG compatibility
	const std::set<std::string> svgSupportedExtensions{
			"svg",
	};

	if(svgSupportedExtensions.find(fileExtension) != svgSupportedExtensions.end())
		return std::unique_ptr<Video::VideoSourceBase>(new Sources::SVG(file, 96));

	//Check ImageMagick compatibility
	const std::set<std::string> magickppSupportedExtensions{
			"jpeg",
			"jpg",
			"bmp",
			"png",
	}; //TODO complete list

	if(magickppSupportedExtensions.find(fileExtension) != magickppSupportedExtensions.end())
		return std::unique_ptr<Video::VideoSourceBase>(new Sources::ImageMagick(file));


	//Check FFmpeg compatibility
	const std::set<std::string> ffSupportedExtensions{
			"mp4",
			"avi",
			"mov",
			"mpeg",
	}; //TODO complete list

	if(ffSupportedExtensions.find(fileExtension) != ffSupportedExtensions.end())
		return std::unique_ptr<Video::VideoSourceBase>(new Sources::FFmpeg(file));

	return std::unique_ptr<Video::VideoSourceBase>(); //No luck :-<
}





}
