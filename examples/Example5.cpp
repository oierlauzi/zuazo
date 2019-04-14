/**
 * DESCRIPTION:
 * Takes an input and diplays it in grayscale with a gain in luminance of 2
 */

/**
 * HOW TO COMPILE:
 * g++ Example5.cpp -o Example5 -std=c++17 -lzuazo -lavutil -lavformat -lavcodec -lswscale -lglfw -lMagick++-6.Q16 -lMagickWand-6.Q16 -lMagickCore-6.Q16
 * HOW TO RUN:
 * ./Example5 <file1> <file2> <file3>
 *
 */

#include <zuazo/Includes.h>

#include <cstdio>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

int main(int argc, char *argv[]){
	//Initialize zuazo
	if(Zuazo::init() != Zuazo::Errors::OK){
		std::cout << "Error initializing zuazo. Aborting" << std::endl;
		std::terminate();
	}

	if(argc != 2){
		std::cout << "You need to specify one input!" << std::endl;
		std::cout << "Usage: " << std::string(argv[0]) << " <file>" << std::endl;
		std::terminate();
	}

	std::unique_ptr<Zuazo::Video::VideoSourceBase> src(Zuazo::videoSourceFromFile(std::string(argv[1])));

	//Create a window for outputting the resulting mix
	//Window only cares about "res" "frameRate" parameters of the given VideoMode
	//A window title can be provided after the VideoMode
	Zuazo::Utils::VideoMode windowVideoMode;
	windowVideoMode.res=Zuazo::Utils::Resolution(1280, 720);
	windowVideoMode.frameRate=Zuazo::Utils::Rational(30.0);
	Zuazo::Consumers::Window window(
			windowVideoMode,
			"Example 5"
	);

	//Create the previously defined side-by-side generator
	Zuazo::Utils::VideoMode fxVideoMode;
	fxVideoMode.pixFmt=Zuazo::Utils::PixelFormats::PIX_FMT_RGB32;
	fxVideoMode.res=Zuazo::Utils::Resolution(1280, 720);

	Zuazo::Processors::Grayscale fx1(fxVideoMode);
	Zuazo::Processors::BrightnessContrast fx2(fxVideoMode);

	fx2.setBrightness(0.75); //A bit brighter

	//Route the signal src -> fx1 -> fx2 -> window
	if(src) fx1.videoIn << src->videoOut;
	fx2.videoIn << fx1.videoOut;
	window.videoIn << fx2.videoOut;


	std::cout << fx2.getShaderLog() << std::endl;

	std::cout << "Program running... Press enter to quit" << std::endl;

	getchar();

	//You should always close the objects before calling end()
	//Deleting them is also OK
	window.close();
	fx1.close();
	fx2.close();

	Zuazo::end();
}
