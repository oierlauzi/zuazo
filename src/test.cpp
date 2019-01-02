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

	//Zuazo::Surface sfc;

	do{
		//draw something
		u_int8_t color[]={
				rand(),
				rand(),
				rand(),
				1
		};

		for(u_int32_t i=200; i<800; i++)
			for(u_int32_t j=200; j<800; j++){
				size_t pos=4 * (i*1920 + j);
				for(u_int32_t h=0; h<4; h++)
					extImg.data[pos+h]=color[h];
			}

		img.copy(extImg);
		win.draw(img);
	//}while(getchar()!='e');
	}while(true);


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
