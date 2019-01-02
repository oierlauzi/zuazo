#include <bits/types/FILE.h>
#include <stdlib.h>
#include <sys/types.h>
#include <cstdio>
#include <cstring>
#include <math.h>

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

	const u_int32_t width=1920;
	const u_int32_t height=1080;

	Zuazo::ExtImage extImg;
	extImg.res={width, height};
	extImg.data=(u_int8_t*)malloc(extImg.getSize());

	//Zuazo::Surface sfc;
	double i=0;
	do{
		//draw something
		const u_int32_t radio=100;
		u_int32_t posX=(0.5 + cos(i + 0)/4)*width;
		u_int32_t posY=(0.5 + sin(i + 0)/4)*height;
		u_int8_t color[]={
				(0.5 + sin(i*1.5 + 0		)/2) * 0xff,
				(0.5 + sin(i*1.5 + M_PI*2/3	)/2) * 0xff,
				(0.5 + sin(i*1.5 + M_PI*4/3	)/2) * 0xff,
				1
		};

		for(u_int32_t i=posY-radio; i<posY+radio; i++){
			u_int32_t row=i-posY;
			u_int32_t  off=sqrt(radio*radio - row*row);
			for(u_int32_t j=posX-off; j<posX+off; j++){
				size_t pos=4 * (i*1920 + j);
				for(u_int32_t h=0; h<4; h++)
					extImg.data[pos+h]=color[h];
			}
		}

		img.copy(extImg);
		win.draw(img);
		i+=0.01*M_PI;
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
