#include <zuazo/Consumers/Window.h>
#include <zuazo/Stream/Consumer.h>
#include <zuazo/Stream/Source.h>
#include <zuazo/Utils/Resolution.h>
#include <zuazo/Utils/VideoMode.h>
#include <zuazo/Video/VideoSourceBase.h>
#include <zuazo/Zuazo.h>

#include <cstdio>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/**
 * DESCRIPTION:
 * This example shows how to make a simple video-switcher with zuazo.
 * It takes as arguments the desired video input paths and it is able to switch
 * between them by pressing keys 0-9
 */

/**
 * HOW TO COMPILE:
 * g++ Example1.cpp -o Example1 -std=c++17 -lzuazo -lavutil -lavformat -lavcodec -lswscale -lglfw -lMagick++-6.Q16 -lMagickWand-6.Q16 -lMagickCore-6.Q16
 * HOW TO RUN:
 * LD_LIBRARY_PATH=./Example1 <file1> <file2> ... <file10>
 *
 * where file(n) equals a JPG, PNG, SVG, BMP, MP4, AVI... file or a V4L2 input (/dev/video0, /dev/video1 ...)
 */
int main(int argc, char *argv[]){
	//Check if the user has given at least one argument
	//The first position is always filled with the name of the executable
	//So argc (amount of arguments) will equal at least 1.
	//It also needs to be smaller or equal than 11, as there are no more than
	//10 number keys on the keyboard
	if(argc < 2){
		std::cout << "You need to specify at least one input!" << std::endl;
		std::cout << "Usage: " << std::string(argv[0]) << " <file1> <file2> ... <file10>" << std::endl;
		std::terminate();
	}else if(argc > 11){
		std::cout << "More than 10 inputs were given" << std::endl;
		std::cout << "The last " << (argc - 11) << " input(s) will be ignored!" << std::endl;
	}


	//Initialize zuazo
	if(Zuazo::init() != Zuazo::Errors::OK){
		std::cout << "Error initializing zuazo. Aborting" << std::endl;
		std::terminate();
	}

	//Create a vector of pointers to video sources
	std::vector<std::unique_ptr<Zuazo::Video::VideoSourceBase>> videoSources;
	//As the size of the vector is known, its more efficient to allocate it in advance
	videoSources.resize(10);

	//Iterate through the list and create sources with each file
	for(int i=0; (i < argc - 1) && (i < 10); i++){
		const std::string path(argv[i + 1]);
		videoSources[i]=Zuazo::videoSourceFromFile(path); //Create a video source from it

		if(!videoSources[i]){
			std::cout << "Failed to open " << path << std::endl;
		}else{
			std::cout << path << " successfully opened" << std::endl;
		}
	}

	//Create a window for outputting the resulting mix
	//Window only cares about "res" "frameRate" parameters of the given VideoMode
	//A window title can be provided after the VideoMode
	Zuazo::Consumers::Window window(
			Zuazo::Utils::VideoMode{
				.res=Zuazo::Utils::Resolution(1280, 720),
				.frameRate=Zuazo::Utils::Rational(30.0)
			},
			"Example 1"
	);

	//Show instructions on screen
	std::cout << "Use keys 0-9 to switch between inputs" << std::endl;
	std::cout << "Use [Q] to exit" << std::endl;
	std::cout << "Remember to push [ENTER] after your selection" << std::endl;

	//Main loop
	char key;
	do{
		//Register a key press
		key=getchar();

		//Check if the key is within the range
		if(key >= '0' && key <= '9'){
			int idx=key - '0'; //Translate from ASCII to an integer

			//Check if a valid source is available with that index
			if(videoSources[idx]){
				window.videoIn << videoSources[idx]->videoOut; //It's this easy to switch inputs in zuazo
			}else
				window.videoIn << nullptr;
		}
	}while(key != 'q');

	//Delete the sources
	for(auto& source : videoSources){
		source.reset();
	}

	//You should always close the objects before calling end()
	//Deleting them is also OK
	window.close();

	Zuazo::end();
}
