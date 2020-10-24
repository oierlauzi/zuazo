# Zuazo (WIP)

## Description

Zuazo is library for manipulating real-time video. It's designed to take advantege of the GPU by offloading most of the work to Vulkan. It features a simple but powerful signal routing, easy to use timing and a RAII intensive interface. Core funtionality can be expanded using modules, which are available on my GitHub page.

## Requirements

- Vulkan 1.0 compatible video card

- Debian based OS

## Install instructions

### Debian 10 and Ubuntu 18.04 LTS

Install build tools:

``sudo apt-get install git cmake g++``

If you want to generate a doxygen document in HTML for Zuazo install doxygen and graphviz:

``sudo apt-get install doxygen graphviz``

Install 3rd party dependencies

``sudo apt-get install vulkan-tools glslang-tools libvulkan-dev vulkan-validationlayers-dev libglm-dev ``

Download the repository from GitHub
 
``git clone https://github.com/oierlauzi/zuazo.git``

Build it

``cd zuazo``
 
``mkdir build``

``cmake CMakeLists.txt -B build/``

``cd build``

``make ``

Alternatively run ``make -j##`` where ## is the number of threads in your system, to speed up the building process

``sudo make install``

Optionally you can generate the Doxygen documentation

``make doxygen``

This documentation will be placed in ``doc/doxygen/html``. To access it open ``index.html`` with your browser.

You're done!

## Examples

In the examples folder you can find the source code for several applications that show the usage of the API. Inside each source file there is a brief description, build instructions, and instructions for executing the program.

## Linking your aplication

To link your application against zuazo, you need to add the following arguments to the linker

``-lzuazo``

