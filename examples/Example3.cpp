/**
 * DESCRIPTION:
 * Renders in 3D space a random video sources, with a random position, rotation and size
 * Sometimes adding a new layer won't make much of a difference in the final rendering.
 * This is due to the layer is being placed out of sight, or perpendicular to the projection plane
 *
 */

/**
 * HOW TO COMPILE:
 * g++ Example3.cpp -o Example3 -std=c++17 -lzuazo -lavutil -lavformat -lavcodec -lswscale -lglfw -lMagick++-6.Q16 -lMagickWand-6.Q16 -lMagickCore-6.Q16
 * HOW TO RUN:
 * ./Example3 <file1> <file2> ... <filen>
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

#define RAND_IN_RANGE(min, max) ((int)(min) + (rand() % ((int)(max) - (int)(min))))

int main(int argc, char *argv[]){
	//Initialize zuazo
	if(Zuazo::init() != Zuazo::Errors::OK){
		std::cout << "Error initializing zuazo. Aborting" << std::endl;
		std::terminate();
	}

	if(argc < 2){
		std::cout << "You need to specify at least one input!" << std::endl;
		std::cout << "Usage: " << std::string(argv[0]) << " <file1> <file2> ... <filen>" << std::endl;
		std::terminate();
	}

	//Initialize the random number generator
	srand (time(NULL));

	//Create a vector of pointers to video sources
	std::vector<std::unique_ptr<Zuazo::Video::VideoSourceBase>> videoSources;
	//As the size of the vector is known, its more efficient to allocate it in advance
	videoSources.resize(argc - 1);

	Zuazo::begin();

	//Iterate through the list and create sources with each file
	for(int i=0; i < argc - 1; i++){
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
	Zuazo::Utils::VideoMode windowVideoMode;
	windowVideoMode.res=Zuazo::Utils::Resolution(1280, 720);
	windowVideoMode.frameRate=Zuazo::Utils::Rational(30.0);
	Zuazo::Consumers::Window window(
			windowVideoMode,
			"Example 3"
	);

	//Create a Compositor for mixing the video sources
	//Compositor only cares of parameters pixel format and resolution
	Zuazo::Utils::VideoMode compVideoMode;
	compVideoMode.pixFmt=Zuazo::Utils::PixelFormats::PIX_FMT_RGB32;
	compVideoMode.res=Zuazo::Utils::Resolution(1280, 720);
	Zuazo::Processors::Compositor composer(compVideoMode);

	//Feed the composer into the window
	window.videoIn << composer.videoOut;

	Zuazo::end();

	//Get all needed variables
	const float zMax=composer.getNearClip();
	const float zMin=-composer.getNearClip();
	const float xMax=composer.getResolution().width / 2;
	const float xMin=-xMax;
	const float yMax=composer.getResolution().height / 2;
	const float yMin=-yMax;
	const float wMax=xMax;
	const float wMin=xMax / 2;
	const float hMax=yMax;
	const float hMin=yMax / 2;

	//Show the instructions
	std::cout << "Use [+] to add a new random layer" << std::endl;
	std::cout << "Use [-] to delete the newest layer" << std::endl;
	std::cout << "Use [Q] to exit" << std::endl;

	//Main loop
	char key;
	do{
		//Register a key press
		key=getchar();

		Zuazo::Context ctx;
		auto layers=composer.getLayers();

		switch(key){
		case '-':
			layers.pop_back();

			std::cout << "Deleting a layer:" << std::endl;
			std::cout << "Layer count:	" << layers.size() << std::endl << std::endl;
			break;
		case '+':
			//Add a layer
            
			//Create a rectangle with a random size
			const float width=RAND_IN_RANGE(wMin, wMax);
			const float height=RAND_IN_RANGE(hMin, hMax);
			const Zuazo::Graphics::Rectangle rect={
				.size=Zuazo::Utils::Vec2f(width, height)
			};

			//Create a layer for the rectangle
			std::shared_ptr<Zuazo::Processors::Compositor::VideoLayer> layer(
					new Zuazo::Processors::Compositor::VideoLayer(rect)
			);

			//Choose a random input for the layer
			u_int32_t inputIndex=(u_int32_t)RAND_IN_RANGE(0, argc - 1);
			if(videoSources[inputIndex])
				layer->videoIn << videoSources[inputIndex]->videoOut;

			//Choose a random position in space
			Zuazo::Utils::Vec3f position(
					RAND_IN_RANGE(xMin, xMax),
					RAND_IN_RANGE(yMin, yMax),
					RAND_IN_RANGE(zMin, zMax)
			);
			layer->setPosition(position);

			//Choose a random rotation
			Zuazo::Utils::Vec3f rotation(
					glm::radians((float)(rand() % 360)),
					glm::radians((float)(rand() % 360)),
					glm::radians((float)(rand() % 360))
			);
			layer->setRotation(rotation);

			//Insert the newly create layer into the composer
			layers.emplace_back(layer);

			std::cout << "Adding a layer:" << std::endl;
			std::cout << "Input index: 	" << inputIndex << std::endl;
			std::cout << "Resolution: 	" << width << "x" << height << std::endl;
			std::cout << "Position: 	(" << position.x << ", " << position.y << ", " << position.z << ")"<< std::endl;
			std::cout << "Rotation: 	(" << glm::degrees(rotation.x) << "º, " << glm::degrees(rotation.y) << "º, " << glm::degrees(rotation.z) << "º)" << std::endl;
			std::cout << "Layer count:	" << layers.size() << std::endl << std::endl;
			break;
		}

		composer.setLayers(layers);
	}while(key != 'q');

	Zuazo::begin();

	for(auto& source : videoSources){
		source.reset();
	}

	//You should always close the objects before calling end()
	//Deleting them is also OK
	window.close();
	composer.close();

	Zuazo::end();

	Zuazo::terminate();
}
