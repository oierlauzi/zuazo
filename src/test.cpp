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
#include "Zuazo/UpdateOrder.h"
#include "Zuazo/Timing/Chronometer.h"
#include "Zuazo/Utils/Color.h"
#include "Zuazo/Utils/Rational.h"
#include "Zuazo/Zuazo.h"
#include "Zuazo/Timing/TimeInterval.h"
#include "Zuazo/Graphics/GL/FrameBuffer.h"
#include "Zuazo/Graphics/GL/VertexArray.h"
#include "Zuazo/Graphics/Context.h"
#include "Zuazo/Graphics/Frame.h"
#include "Zuazo/Media/Consumers/Window.h"
#include "Zuazo/Media/Media.h"
#include "Zuazo/Media/Sources/V4L2.h"
#include "Zuazo/Media/Sources/FFmpeg.h"
#include "Zuazo/Media/Sources/SVG.h"

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
	Zuazo::Media::Consumers::Window win(
			Zuazo::Utils::Resolution(1280, 720),
			Zuazo::Utils::Rational(30, 1),
			"Window with delay"
	);

	Zuazo::Media::Sources::V4L2 webcam("/dev/video0");
	const std::set<Zuazo::Media::Sources::V4L2::VideoMode>& vidModes=webcam.getVideoModes();
	for(const Zuazo::Media::Sources::V4L2::VideoMode& vidMode : vidModes){
		printf("%ux%u @ %gHz\n",
				vidMode.resolution.width,
				vidMode.resolution.height,
				(double)vidMode.interval.den / vidMode.interval.num);
	}

	win<<webcam;

	/*auto screens=Zuazo::Media::Consumers::Window::Screen::getScreens();
	sleep(2);
	printf("Setting fullscreen\n");
	win.setFullScreen(*(screens.begin()));
	sleep(2);
	printf("Setting windowed\n");
	win.setWindowed();*/

	getchar();
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

	using DelayD=Zuazo::Stream::Delay<double>;

	class SourceExample :
			public Zuazo::Stream::Source<double>,
			public Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UpdateOrder::FIRST>{
		public:
		SourceExample(double framerate) :
			Zuazo::Stream::Source<double>(),
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UpdateOrder::FIRST>(Zuazo::Utils::Rational(framerate)){

		}

		void open() override{
			Zuazo::Stream::Source<double>::open();
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UpdateOrder::FIRST>::open();
		}

		void close() override{
			Zuazo::Stream::Source<double>::close();
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UpdateOrder::FIRST>::close();
		}

		virtual void update() const{
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

		void open() override{
			Zuazo::Stream::Consumer<double>::open();
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UpdateOrder::LAST>::open();
		}

		void close() override{
			Zuazo::Stream::Consumer<double>::close();
			Zuazo::Timing::PeriodicUpdate<Zuazo::Timing::UpdateOrder::LAST>::close();
		}

		virtual void update() const{
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
	DelayD	del(Zuazo::Timing::TimeInterval(0));
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
