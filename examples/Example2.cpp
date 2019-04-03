/**
 * DESCRIPTION:
 * This example shows the usage of classes which inherit from Timing::NonLinear
 * We are using a video file (MP4, AVI...) decoded by FFmpeg.
 * This clip can be played in different play speeds, paused, jogged, looped...
 */

/**
 * HOW TO COMPILE:
 * g++ Example2.cpp -o Example2 -std=c++17 -lzuazo -lavutil -lavformat -lavcodec -lswscale -lglfw -lMagick++-6.Q16 -lMagickWand-6.Q16 -lMagickCore-6.Q16
 * HOW TO RUN:
 * ./Example2 <video file>
 */

#include <zuazo/Includes.h>

#include <cstdio>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main(int argc, char *argv[]){
	//We need to have 2 arguments passed (1 is always filled with the name of the executable)
	if(argc != 2){
		std::cout << "Wrong arguments were given" << std::endl;
		std::cout << "Usage: " << std::string(argv[0]) << " <video file>" << std::endl;
		std::terminate();
	}

	//Initialize zuazo
	if(Zuazo::init() != Zuazo::Errors::OK){
		std::cout << "Error initializing zuazo. Aborting" << std::endl;
		std::terminate();
	}

	//Create the player for the video clip
	std::string path(argv[1]);
	Zuazo::Sources::FFmpeg player(path);

	//By default is playing. Pause it and rewind
	player.setState(Zuazo::Sources::FFmpeg::States::Paused);
	player.gotoTime(std::chrono::seconds(0));

	//Create a window for outputting the resulting mix
	//Window only cares about "res" "frameRate" parameters of the given VideoMode
	//A window title can be provided after the VideoMode
	Zuazo::Consumers::Window window(
			Zuazo::Utils::VideoMode{
				.res=Zuazo::Utils::Resolution(1280, 720),
				.frameRate=Zuazo::Utils::Rational(30.0)
			},
			"Example 2"
	);

	window.videoIn << player.videoOut; //Show the player on the recently created window

	//Show instructions on screen
	std::cout << "Use [P] to play/pause" << std::endl;
	std::cout << "Use [L] to toggle repeat" << std::endl;
	std::cout << "Use [S] [#] to set play speed (where # represents the play speed)" << std::endl;
	std::cout << "Use [N] to go to the next frame" << std::endl;
	std::cout << "Use [B] to go to the previous frame" << std::endl;
	std::cout << "Use [R] to rewind" << std::endl;
	std::cout << "Use [Q] to exit" << std::endl;
	std::cout << "Remember to push [ENTER] after your selection" << std::endl;

	//Calculate the frame interval
	Zuazo::Utils::TimeInterval frameInterval(1 / player.getFramerate());

	//Main loop
	char key;
	do{
		//Register a key press
		key=getchar();

		switch(key){
		case 'p':
			if(player.getState() == Zuazo::Sources::FFmpeg::States::Playing){
				//It was on play mode. pause it
				player.setState(Zuazo::Sources::FFmpeg::States::Paused);
				std::cout << "Player's state set to paused" << std::endl;
			}else{
				//It was paused. Set it on playing mode
				player.setState(Zuazo::Sources::FFmpeg::States::Playing);
				std::cout << "Player's state set to playing" << std::endl;
			}
			break;

		 case 'l':
			 //Toggle repeat
			 player.setRepeat(!player.getRepeat());

			 if(player.getRepeat())
				 std::cout << "Repeat was turned on" << std::endl;
			 else
				 std::cout << "Repeat was turned off" << std::endl;

			 break;

		 case 's':
			 double speed;
			 std::cin >> speed;
			 player.setPlaySpeed(speed);

			 std::cout << "Play speed was set to x" << player.getPlaySpeed() << std::endl;

			 break;

		 case 'n':
			 player.addTime(frameInterval);

			 std::cout << "Advanced " << frameInterval.count() << "us"<< std::endl;
			 break;

		 case 'b':

			 player.subsTime(frameInterval);
			 std::cout << "Rewinded " << frameInterval.count() << "us"<< std::endl;
			 break;

		 case 'r':
			 player.gotoTime(std::chrono::seconds(0));

			 std::cout << "Rewinded to the begining" << std::endl;
			 break;
		}


	}while(key != 'q');



	//You should always close the objects before calling end()
	//Deleting them is also OK
	window.close();
	player.close();

	Zuazo::end();
}
