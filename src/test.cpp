#include <glad/glad.h>
#include <cstdio>
#include <exception>
#include <iostream>
#include <map>
#include <set>

#include "Zuazo/Consumers/Window.h"
#include "Zuazo/Sources/FFmpeg.h"
#include "Zuazo/Sources/SVG.h"
#include "Zuazo/Sources/V4L2.h"
#include "Zuazo/Utils/Rational.h"
#include "Zuazo/Utils/Resolution.h"
#include "Zuazo/Zuazo.h"

#define TEST1
//#define TEST2
//#define TEST3
//#define TEST4
//#define TEST5
//#define TEST6
//#define TEST7

int main(void){
	/*
	 * 		INITIALIZATION
	 */

	if(Zuazo::init() != Zuazo::Errors::OK){
		printf("Error initializing\n");
		std::terminate();
	}

	printf("OpenGL: %d.%d\n", GLVersion.major, GLVersion.minor);

	#ifdef TEST1
	/*
	 * 		TEST 1:
	 * 		General testing
	 */

	{
	Zuazo::Consumers::Window win(
			Zuazo::Utils::Resolution(1280, 720),
			Zuazo::Utils::Rational(30, 1),
			"Window with delay"
	);

	Zuazo::Sources::FFmpeg vid("/home/oierlauzi/Bideoak/prueba1.mp4");

	Zuazo::Sources::SVG svg("/home/oierlauzi/Irudiak/color_bars.svg", 96);
	Zuazo::Sources::V4L2 webcam1("/dev/video0");
	Zuazo::Sources::V4L2 webcam2("/dev/video2");

	webcam1.setVideoMode(*(--webcam1.getVideoModes().end()));
	//webcam2.setVideoMode(*(--webcam2.getVideoModes().end()));

	char a;
	do{
		a=getchar();

		switch(a){
		case '1':
			win.videoIn<<webcam1.videoOut;
			break;
		case '2':
			win.videoIn<<webcam2.videoOut;
			break;
		case '3':
			win.videoIn<<vid.videoOut;
			break;
		case '4':
			win.videoIn<<svg.videoOut;
			break;
		case 'e':
			break;
		}
	}while(a != 'e');
	}



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
	scanf("%lf", &d1);VideoSrcTest
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

	class TimingExample : public Zuazo::Timing::PeriodicUpdate<Zuazo::UpdateOrder::INPUT>{
	public:
		TimingExample(Zuazo::Utils::Rational& a) :
			Zuazo::Timing::PeriodicUpdate<Zuazo::UpdateOrder::INPUT>(Zuazo::Utils::Rational(a)){

		}

		void update() const{
			printf("Updating\n");
		}
	};

	Zuazo::Utils::Rational r4(29.97);
	Zuazo::Utils::Rational r5(30);
	TimingExample t4(r4);
	TimingExample t5(r5);
	getchar();

#endif

#ifdef TEST4
/*
 * 		TEST 4:
 *		Source / Consumer testing
 */

	using DelayExample=Zuazo::Stream::Delay<double>;
	using DummyPadExample=Zuazo::Stream::DummyPad<double>;

	class SourceExample :
			public Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UPDATE_ORDER_SOURCE>,
			public Zuazo::ZuazoBase
	{
	public:
		SourceExample(double framerate) :
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UPDATE_ORDER_SOURCE>(Zuazo::Utils::Rational(framerate))
		{
			open();
		}

		~SourceExample(){
			close();
		}

		void open() override{
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UPDATE_ORDER_SOURCE>::enable();
		}

		void close() override{
			src.reset();
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UPDATE_ORDER_SOURCE>::disable();
		}

		virtual void update() const{
			static double i=0;

			std::unique_ptr<const double> d(new double(i));
			src.push(std::move(d));
			i+=1;

			printf("Sending\n");
		}

		Zuazo::Stream::SourcePad<double, SourceExample>		src;
	};

	class ConsumerExample :
			public Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UPDATE_ORDER_CONSUMER>,
			public Zuazo::ZuazoBase
	{
	public:
		ConsumerExample(double framerate) :
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UPDATE_ORDER_CONSUMER>(Zuazo::Utils::Rational(framerate))
		{
			open();
		}

		~ConsumerExample(){
			close();
		}

		void open() override{
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UPDATE_ORDER_CONSUMER>::enable();
		}

		void close() override{
			cons.reset();
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UPDATE_ORDER_CONSUMER>::disable();
		}

		virtual void update() const{
			Zuazo::Utils::TimePoint ts;
			std::shared_ptr<const double> ptr=cons.get();
			if(ptr)
				printf("On update: %g\n", *ptr);
			else
				printf("On update: nullptr\n");

		}

		Zuazo::Stream::ConsumerPad<double, ConsumerExample>		cons;
	};


	ConsumerExample cons(30);
	SourceExample src(30);
	DummyPadExample pad;
	DelayExample delay(std::chrono::seconds(1));
	//DelayD	del(Zuazo::Timing::TimeInterval(0));
	//del<<src;

	delay<<src.src;
	cons.cons<<pad;

	pad<<src.src;
	getchar();
	pad<<delay;
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


	Zuazo::Graphics::UniqueContext ctx(Zuazo::Graphics::Context::mainCtx); //Get a context

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


	using Frame=Zuazo::Graphics::Frame;
	Frame* fr[noFrames];

	printf("*********************\n");
	printf("*    FIRST ROUND    *\n");
	printf("*********************\n");
	for(size_t i=0; i<noFrames; i++){
		Zuazo::Timing::Chronometer chrono;
		chrono.start();
		fr[i]=new Frame(pixbuff);
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
		fr[i]=new Frame(pixbuff);
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
