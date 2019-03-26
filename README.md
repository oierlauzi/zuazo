# Zuazo
A hardware accelerated real-time video manipulation library for C++

## Install instructions
### Tested on Debian 10
 1. Install build tools
 
	`sudo apt-get install git cmake g++`

 2.  Install dependencies
 
	`sudo apt-get install libavutil libavcodec libavformat libswscale libglfw3 libmagick++-6.q16-dev`
	 
 3.  Download the repository from github
 
	`git clone https://github.com/oierlauzi/zuazo.git`
	
 4. Build it
 
 	`cd Zuazo`
	 
	`cmake CMakeLists.txt`
	 
	`make #Run this with -j<number of cores in your system> to take advantage of them`
	 
	`sudo make install`
	 
 5. You're done!
