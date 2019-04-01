# Zuazo (WIP)

## Description

Zuazo is library for manipulating real-time video. It's designed to take advantege of the GPU by offloading most of the work to OpenGL. It features a simple but powerful signal routing, compatibility with several video sources such as a video file, Video for Linux 2 (V4L2) device, still images, ImageMagick images, SVG files... Currently it can combine several video sources by using the Compositor class.  It has been written in C++17, including the API so it is *NOT* compatible with older versions of C++.

## Requirements

- OpenGL 3.2+ compatible video card

- Debian based OS

## Install instructions

### Debian 10

Install build tools:

``sudo apt-get install git cmake g++``

Install 3rd party dependencies

``sudo apt-get install libavutil libavcodec libavformat libswscale libglfw3 libmagick++-6.q16-dev libglm-dev ``

Download the repository from github
 
``git clone https://github.com/oierlauzi/zuazo.git``

Build it

 ``cd Zuazo``

``cmake CMakeLists.txt``

``make ``

Alternatively run ``make -j##`` where ## is the number of cores in your system, to speed up the building process

``sudo make install``

You're done!

## Examples

In the examples folder you can find the source code for several application that show the usage of the API. Inside each source file there is a brief description, build instructions, and instructions for executing the program.

## Linking your aplication

To link your application against zuazo, you need to add the following arguments to the linker

``-lzuazo -lavutil -lavformat -lavcodec -lswscale -lglfw -lMagick++-6.Q16 -lMagickWand-6.Q16 -lMagickCore-6.Q16``

