/**
 * DESCRIPTION:
 * This example shows the usage of several effects
 */

/**
 * HOW TO COMPILE:
 * g++ Example5.cpp -o Example5 -std=c++17 -lzuazo -lavutil -lavformat -lavcodec -lswscale -lglfw -lMagick++-6.Q16 `/usr/lib/x86_64-linux-gnu/ImageMagick-6.9.10/bin-q16/Magick++-config --cppflags --cxxflags --ldflags --libs`
 * HOW TO RUN:
 * ./Example5 <file1>
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

	Zuazo::begin();

	std::unique_ptr<Zuazo::Video::VideoSourceBase> src(Zuazo::videoSourceFromFile(std::string(argv[1])));

	//Create 5 windows
	//In the first the source will be shown in black and white
	//In the second it will be shown with an increase in saturation and hue shift
	Zuazo::Utils::VideoMode windowVideoMode;
	windowVideoMode.res=Zuazo::Utils::Resolution(1280, 720);
	windowVideoMode.frameRate=Zuazo::Utils::Rational(30.0);
	Zuazo::Consumers::Window window0(
			windowVideoMode,
			"Example 5: Original"
	);

	Zuazo::Consumers::Window window1(
			windowVideoMode,
			"Example 5: Delayed 1s"
	);

	Zuazo::Consumers::Window window2(
			windowVideoMode,
			"Example 5: Inverted"
	);

	Zuazo::Consumers::Window window3(
			windowVideoMode,
			"Example 5: Grayscale"
	);

	Zuazo::Consumers::Window window4(
			windowVideoMode,
			"Example 5: Hue shifted"
	);

	Zuazo::Consumers::Window window5(
			windowVideoMode,
			"Example 5: Chroma key"
	);

		Zuazo::Consumers::Window window6(
			windowVideoMode,
			"Example 5: Alpha Visualization"
	);

	//Create the previously defined side-by-side generator
	Zuazo::Utils::VideoMode fxVideoMode;
	fxVideoMode.pixFmt=Zuazo::Utils::PixelFormats::PIX_FMT_RGB32;

	Zuazo::Video::VideoDelay delay(std::chrono::seconds(1));
	Zuazo::Processors::Invert fx0(fxVideoMode);
	Zuazo::Processors::Grayscale fx1(fxVideoMode);
	Zuazo::Processors::BrightnessContrast fx2(fxVideoMode);
	Zuazo::Processors::HueSaturationLuminosity fx3(fxVideoMode);
	Zuazo::Processors::ChromaKey fx4(fxVideoMode);
	Zuazo::Processors::Alpha2Luma fx5(fxVideoMode);

	fx2.setBrightness(0.75); //A bit brighter

	fx3.setSaturation(2.0); //More saturation
	fx3.setHue(+90.0); //90º of hue shift

	//Route the signal: 
	//      +-> window0 (Original)
	//      |
	//		+-> delay -> window1 (Delayed 1s)
	//		|
	//		+-> fx0 -> window2 (Inverted)
	//		|
	// src -+-> fx1 -> fx2 -> window3 (Grayscale)
	//	    |
	//	    +-> fx3 -> window4 (Hue shifted)
	//		|
	//		+-> fx4 -+-> window5 (Chorma key)
	//				 |
	//		         +-> fx5 -> window6 (Alpha visualization)

	if(src) window0.videoIn << src->videoOut;

	if(src) delay.in << src->videoOut;
	window1.videoIn << delay.out;

	if(src) fx0.videoIn << src->videoOut;
	window2.videoIn << fx0.videoOut;

	if(src) fx1.videoIn << src->videoOut;
	fx2.videoIn << fx1.videoOut;
	window3.videoIn << fx2.videoOut;

	if(src) fx3.videoIn << src->videoOut;
	window4.videoIn << fx3.videoOut;

	if(src) fx4.videoIn << src->videoOut;
	window5.videoIn << fx4.videoOut;
	fx5.videoIn << fx4.videoOut;
	window6.videoIn << fx5.videoOut;

	Zuazo::end();

	std::cout << "Program running... Press enter to quit" << std::endl;

	getchar();

	Zuazo::begin();

	//You should always close the objects before calling end()
	//Deleting them is also OK
	window0.close();
	window1.close();
	window2.close();
	window3.close();
	window4.close();
	delay.close();
	fx0.close();
	fx1.close();
	fx2.close();
	fx3.close();
	fx4.close();
	src.reset(); //Delete the source by reseting its ptr

	Zuazo::end();

	Zuazo::terminate();
}
