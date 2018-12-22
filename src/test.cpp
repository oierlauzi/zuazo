#include <GL/glew.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <list>
#include <string>

#include "../include/Window.h"
#include "../include/Zuazo.h"

#define TEST1

int main(void){
	/*
	 * 		INITIALIZATION
	 */

	Zuazo::init();

	#ifdef TEST1
	/*
	 * 		TEST 1
	 */

	Zuazo::Window win(680, 480, "Ventana");
	std::list<Zuazo::Window::Screen> screens=Zuazo::Window::getAvalibleScreens();

	for(Zuazo::Window::Screen& scr : screens)
		std::cout<<scr.name <<std::endl;

	getchar();

	#endif
	#ifdef TEST2
	/*
	 * 		TEST 2
	 */


	#endif
}
