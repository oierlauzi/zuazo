/**
 * DESCRIPTION:
 * This example shows the scaling filters supported by Zuazo
 */

/**
 * HOW TO COMPILE:
 * g++ Example6.cpp -o Example6 -std=c++17 -lzuazo -lavutil -lavformat -lavcodec -lswscale -lglfw -lMagick++-6.Q16 -lMagickWand-6.Q16 -lMagickCore-6.Q16
 * HOW TO RUN:
 * ./Example6 <file1>
 *
 */

#include <zuazo/Includes.h>

#include <cstdio>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

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

	//Create 3 windows
	//In the first the source will be shown in black and white
	//In the second it will be shown with an increase in saturation and hue shift
	Zuazo::Utils::VideoMode windowVideoMode;
	windowVideoMode.res=Zuazo::Utils::Resolution(1280, 720);
	windowVideoMode.frameRate=Zuazo::Utils::Rational(30.0);
	Zuazo::Consumers::Window window0(
			windowVideoMode,
			"Example 6: Nearest"
	);
	window0.setScalingFilter(zz::Utils::ScalingFilter::Nearest);

	Zuazo::Consumers::Window window1(
			windowVideoMode,
			"Example 6: Bilinear"
	);
	window1.setScalingFilter(zz::Utils::ScalingFilter::Bilinear);

	Zuazo::Consumers::Window window2(
			windowVideoMode,
			"Example 6: Bicubic"
	);
	window2.setScalingFilter(zz::Utils::ScalingFilter::Bicubic);

	if(src){
		window0.videoIn << src->videoOut;
		window1.videoIn << src->videoOut;
		window2.videoIn << src->videoOut;
	} 

	Zuazo::end();

	std::cout << "Program running... Press enter to quit" << std::endl;

	getchar();

	Zuazo::begin();

	//You should always close the objects before calling end()
	//Deleting them is also OK
	window0.close();
	window1.close();
	window2.close();
	src.reset(); //Delete the source by reseting its ptr

	Zuazo::end();

	Zuazo::terminate();
}
