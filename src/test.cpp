#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include "Zuazo/Stream/Consumer.h"
#include "Zuazo/Stream/Delay.h"
#include "Zuazo/Stream/Source.h"
#include "Zuazo/Timing/PeriodicUpdate.h"
#include "Zuazo/Timing/TimePoint.h"
#include "Zuazo/Timing/UpdateOrder.h"
#include "Zuazo/Timing/Chronometer.h"
#include "Zuazo/Utils/Color.h"
#include "Zuazo/Utils/Rational.h"
#include "Zuazo/Zuazo.h"
#include "Zuazo/Timing/TimeUnit.h"
#include "Zuazo/Graphics/GL/FrameBuffer.h"
#include "Zuazo/Graphics/GL/Context.h"
#include "Zuazo/Graphics/Frame.h"

//#define TEST1
//#define TEST2
//#define TEST3
//#define TEST4
//#define TEST5
//#define TEST6
#define TEST7

int main(void){
	/*
	 * 		INITIALIZATION
	 */

	Zuazo::init();

	#ifdef TEST1
	/*
	 * 		TEST 1:
	 * 		General testing
	 */

	//Zuazo::Window win(680, 480, "Ventana");
	/*std::list<Zuazo::Window::Screen> screens=Zuazo::Window::getAvalibleScreens();

	for(Zuazo::Window::Screen& scr : screens)
		std::cout<<scr.name <<std::endl;*/

	/*Zuazo::Image img;

	const u_int32_t width=1000;
	const u_int32_t height=1000;

	Zuazo::ImgBuffer extImg(Zuazo::Resolution(width, height));

	Zuazo::Surface sfc;
	double i=0;
	do{
		//draw something
		const u_int32_t radio=100;
		u_int32_t posX=(0.5 + cos(i + 0)/4)*width;
		u_int32_t posY=(0.5 + sin(i + 0)/4)*height;
		u_int8_t color[]={
				(u_int8_t)((0.5 + sin(i*1.5 + 0		)/2) * 0xff),
				(u_int8_t)((0.5 + sin(i*1.5 + M_PI*2/3	)/2) * 0xff),
				(u_int8_t)((0.5 + sin(i*1.5 + M_PI*4/3	)/2) * 0xff),
				1
		};

		for(u_int32_t i=posY-radio; i<posY+radio; i++){
			u_int32_t row=i-posY;
			u_int32_t  off=sqrt(radio*radio - row*row);
			for(u_int32_t j=posX-off; j<posX+off; j++){
				size_t pos=4 * (i*width + j);
				for(u_int32_t h=0; h<4; h++)
					extImg.data[pos+h]=color[h];
			}
		}

		img.copy(extImg);
		sfc.copy(img);
		win.show(sfc);
		i+=0.01*M_PI;
	//}while(getchar()!='e');
	}while(true);*/

	#endif

	#ifdef TEST2
	/*
	 * 		TEST 2:
	 * 		Rational number class test
	 */


	/*
	 * Test of the rational
	 */

	double d1, d2;
	printf("Double: ");
	scanf("%lf", &d1);
	printf("Double: ");
	scanf("%lf", &d2);

	Zuazo::Utils::Rational r1(d1);
	Zuazo::Utils::Rational r2(d2);
	Zuazo::Utils::Rational r3=r1*d2;

	printf("%ud/%ud\n", r1.num, r1.den);
	printf("%ud/%ud\n", r2.num, r2.den);
	printf("%ud/%ud\n", r3.num, r3.den);



	#endif

#ifdef TEST3
/*
 * 		TEST 3:
 *		Timing testing
 */

	class TimingExample : public Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UpdateOrder::FIRST>{
	public:
		TimingExample(Zuazo::Utils::Rational& a) :
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UpdateOrder::FIRST>(Zuazo::Utils::Rational(a)){

		}

		void update(){
			printf("Updating\n");
		}
	};

	Zuazo::Utils::Rational r4(29.97);
	Zuazo::Utils::Rational r5(30);
	TimingExample t4(r2);
	TimingExample t5(r3);
	getchar();

#endif

#ifdef TEST4
/*
 * 		TEST 3:
 *		Source / Consumer testing
 */

	using DelayD=Zuazo::Stream::Delay<double>;

	class SourceExample :
			public Zuazo::Stream::Source<double>,
			public Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UpdateOrder::FIRST>{
		public:
		SourceExample(double framerate) :
			Zuazo::Stream::Source<double>(),
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UpdateOrder::FIRST>(Zuazo::Utils::Rational(framerate)){

		}

		virtual void update(){
			static double i=0;

			std::unique_ptr<const double> d(new double(i));
			push(d);
			i+=1;

			printf("Sending\n");
		}
	};

	class ConsumerExample :
			public Zuazo::Stream::Consumer<double>,
			public Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UpdateOrder::LAST>{
		public:
		ConsumerExample(double framerate) :
			Zuazo::Stream::Consumer<double>(),
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UpdateOrder::LAST>(Zuazo::Utils::Rational(framerate)){

		}

		virtual void update(){
			Zuazo::Timing::TimePoint ts;
			std::shared_ptr<const double> ptr=get(&ts);
			if(ptr)
				printf("On update: %g \tTime since epoch: %ld\n", *ptr, ts.time_since_epoch().count());
			else
				printf("On update: nullptr\n");

		}
	};

	ConsumerExample cons(30);
	SourceExample src(30);
	DelayD	del(Zuazo::Timing::TimeUnit(0));
	del<<src;
	cons<<del;

	getchar();
	cons<<nullptr;
	getchar();
	cons<<del;
	getchar();
#endif

#ifdef TEST5
/*
 * 		TEST 5:
 *		COLOR Testing
 */

	Zuazo::Utils::Color color("#abcdef12");

	std::cout << (std::string)color << "\n";

#endif


#ifdef TEST6
/*
 * 		TEST 6:
 *		GL Testing
 */

	//Definition of constants
	const u_int32_t width=1920;
	const u_int32_t height=1080;
	const Zuazo::Utils::PixelTypes pixtype=Zuazo::Utils::PixelTypes::RGBA;
	const size_t size=width * height * Zuazo::Utils::PIXEL_SIZE<pixtype>;


	Zuazo::Graphics::GL::UniqueContext ctx(Zuazo::Graphics::GL::Context::mainCtx); //Get a context

	u_int8_t data[size];

	//Set some values
	for(size_t i=0; i<size; i+=4){
		data[i + 0]=abs(sin(i * 1 * M_PI/(width*4))) * 0xff;
		data[i + 1]=abs(sin(i * 2 * M_PI/(width*4))) * 0xff;
		data[i + 2]=abs(sin(i * 3 * M_PI/(width*4))) * 0xff;
		data[i + 3]=0xff;
	}

	//Create a pixel buffer with the data
	Zuazo::Graphics::GL::PixelBuffer<pixtype> pixbuff={
			Zuazo::Utils::Resolution(width, height),
			data
	};

	Zuazo::Graphics::GL::PixelUnpackBuffer<pixtype> pbo(pixbuff); //Upload the data
	Zuazo::Graphics::GL::Texture2D<pixtype> tex(pbo); //Create a texture

	printf("Error: %d\n", glGetError());

	//Dowload the texture
	tex.getImage(&pixbuff);

	//Write to disk
	FILE * f_salida=fopen("Prueba.rgba", "w+");
	fwrite(pixbuff.data, size, 1, f_salida);
	fclose(f_salida);

	getchar();

#endif

#ifdef TEST7
/*
 * 		TEST 7:
 *		Graphics testing
 */

	//Definition of constants
	constexpr Zuazo::Utils::ImageAttributes imgAtt(
			Zuazo::Utils::Resolution(1920, 1080),
			Zuazo::Utils::PixelTypes::RGBA
	);
	constexpr size_t size=imgAtt.size();

	constexpr size_t noFrames=100;


	Zuazo::Graphics::GL::UniqueContext ctx(Zuazo::Graphics::GL::Context::mainCtx); //Get a context

	u_int8_t data[size];

	//Set some values
	for(size_t i=0; i<size; i+=4){
		data[i + 0]=abs(sin(i * 1 * M_PI/(imgAtt.res.width*4))) * 0xff;
		data[i + 1]=abs(sin(i * 2 * M_PI/(imgAtt.res.width*4))) * 0xff;
		data[i + 2]=abs(sin(i * 3 * M_PI/(imgAtt.res.width*4))) * 0xff;
		data[i + 3]=0xff;
	}

	//Create a pixel buffer with the data
	Zuazo::Utils::ImageBuffer pixbuff={
			imgAtt,
			data
	};



	Zuazo::Graphics::Frame* fr[noFrames];

	printf("*********************\n");
	printf("*    FIRST ROUND    *\n");
	printf("*********************\n");
	for(size_t i=0; i<noFrames; i++){
		Zuazo::Timing::Chronometer chrono;
		chrono.start();
		fr[i]=new Zuazo::Graphics::Frame(pixbuff);
		chrono.end();
		printf("Uploading... Elapsed: %ld\n", chrono.getElapsed().count());
	}

	for(size_t i=0; i<noFrames; i++){
		Zuazo::Timing::Chronometer chrono;
		chrono.start();
		fr[i]->getTexture();
		chrono.end();
		printf("Texturing... Elapsed: %ld\n", chrono.getElapsed().count());
	}

	for(size_t i=0; i<noFrames; i++){
		delete fr[i];
	}

	printf("*********************\n");
	printf("*    SECOND ROUND   *\n");
	printf("*********************\n");
	for(size_t i=0; i<noFrames; i++){
		Zuazo::Timing::Chronometer chrono;
		chrono.start();
		fr[i]=new Zuazo::Graphics::Frame(pixbuff);
		chrono.end();
		printf("Uploading... Elapsed: %ld\n", chrono.getElapsed().count());
	}

	for(size_t i=0; i<noFrames; i++){
		Zuazo::Timing::Chronometer chrono;
		chrono.start();
		fr[i]->getTexture();
		chrono.end();
		printf("Texturing... Elapsed: %ld\n", chrono.getElapsed().count());
	}


	for(size_t i=0; i<size; i++){
		data[i]=0x00;
	}

	fr[0]->getTexture().getImage(&pixbuff);

	//Write to disk
	FILE * f_salida=fopen("Prueba.rgba", "w+");
	fwrite(pixbuff.data, size, 1, f_salida);
	fclose(f_salida);

	//getchar();

#endif
	/*
	 * 		TERMINATION
	 */

	Zuazo::end();

	std::cout << "End \n";

	return 0;
}
