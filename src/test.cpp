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
#include "Zuazo/Video/Video.h"

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

	Zuazo::init();

	#ifdef TEST1
	/*
	 * 		TEST 1:
	 * 		General testing
	 */

	class VideoSrcTest : public Zuazo::Video::VideoSource{
	public:
		void push(){
			std::unique_ptr<const Zuazo::Graphics::Frame> fr(new Zuazo::Graphics::Frame());
			Zuazo::Video::VideoSource::push(fr);
		}
	};

	class VideoConTest : public Zuazo::Video::VideoConsumer{
	public:
		std::shared_ptr<const Zuazo::Graphics::Frame> get(){
			return Zuazo::Video::VideoConsumer::get();
		}
	};

	VideoSrcTest src;
	VideoConTest con;
	con<<src;

	src.push();
	printf("%p\n", con.get().get());

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
