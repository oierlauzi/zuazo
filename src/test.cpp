#include <bits/types/FILE.h>
#include <stdlib.h>
#include <sys/types.h>
#include <cstdio>
#include <cstring>

#include "Zuazo/Image.h"
#include "Zuazo/Primitives.h"
#include "Zuazo/Window.h"
#include "Zuazo/Zuazo.h"


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
	/*std::list<Zuazo::Window::Screen> screens=Zuazo::Window::getAvalibleScreens();

	for(Zuazo::Window::Screen& scr : screens)
		std::cout<<scr.name <<std::endl;*/

	Zuazo::Image img;

	Zuazo::ExtImage extImg;
	extImg.res={1920, 1080};
	extImg.data=(u_int8_t*)malloc(extImg.getSize());

	for(u_int32_t i=0; i<extImg.getSize(); i++)
		extImg.data[i]=-1;

	img.copy(extImg);

	free(extImg.data);

	//Zuazo::Surface sfc(img);

	do{
		win.draw(img);
	}while(getchar()!='e');


	#endif
	#ifdef TEST2
	/*
	 * 		TEST 2
	 */


	#endif

	/*
	 * 		TERMINATION
	 */

	Zuazo::end();
}

