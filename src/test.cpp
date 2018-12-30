#include <sys/types.h>
#include <unistd.h>
#include <cstdio>

#include "Zuazo/Window.h"
#include "Zuazo/Zuazo.h"
#include "Zuazo/Context.h"
#include "Zuazo/Image.h"


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

	Zuazo::Uploader uplo;

	Zuazo::ExtImage img;
	img.res={1920, 1080};
	img.data=(u_int8_t*)malloc(1920*1080*4);

	uplo.upload(img);

	free(img.data);

	Zuazo::Surface sfc(uplo);

	win.draw(sfc);

	getchar();

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

