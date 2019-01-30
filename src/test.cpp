#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>

#include "Zuazo/Stream/Consumer.h"
#include "Zuazo/Stream/Delay.h"
#include "Zuazo/Stream/Source.h"
#include "Zuazo/Timing.h"
#include "Zuazo/Utils/Rational.h"
#include "Zuazo/Zuazo.h"

//#define TEST1
//#define TEST2
//#define TEST3
#define TEST4
//#define TEST5

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

	Zuazo::Window win(680, 480, "Ventana");
	/*std::list<Zuazo::Window::Screen> screens=Zuazo::Window::getAvalibleScreens();

	for(Zuazo::Window::Screen& scr : screens)
		std::cout<<scr.name <<std::endl;*/

	Zuazo::Image img;

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
	}while(true);

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
	//printf("Double: ");
	//scanf("%lf", &d2);

	Zuazo::Rational r1(d1);
	//Zuazo::Rational r2(d2);
	//Zuazo::Rational r3=r1*d2;

	printf("%ld/%ld\n", r1.num, r1.den);
	//printf("%ld/%ld\n", r2.num, r2.den);
	//printf("%ld/%ld\n", r3.num, r3.den);



	#endif

#ifdef TEST3
/*
 * 		TEST 3:
 *		Timing testing
 */

	class TimingExample : public Zuazo::Updateable{
	public:
		TimingExample(Zuazo::Rational& a) : Zuazo::Updateable(Zuazo::Rational(a)){

		}

		void update(){
			printf("Updating\n");
		}
	};

	Zuazo::Rational r1(29.97);
	Zuazo::Rational r2(30);
	TimingExample t1(r1);
	TimingExample t2(r2);
	getchar();

#endif

#ifdef TEST4
/*
 * 		TEST 3:
 *		Source / Consumer testing
 */

	using DelayD=Zuazo::Stream::Delay<double>;

	class SourceExample : public Zuazo::Stream::Source<double>, Zuazo::Updateable{
		public:
		SourceExample(double framerate) : Zuazo::Stream::Source<double>(), Zuazo::Updateable(Zuazo::Utils::Rational(framerate)){

		}

		virtual void update(){
			static double i=0;

			std::unique_ptr<const double> d(new double(i));
			push(d);
			i+=1;

			printf("Sending\n");
		}
	};

	class ConsumerExample : public Zuazo::Stream::Consumer<double>, Zuazo::Updateable{
		public:
		ConsumerExample(double framerate) : Zuazo::Stream::Consumer<double>(), Updateable(Zuazo::Utils::Rational(framerate)){

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

	SourceExample src(30);
	ConsumerExample cons(30);
	DelayD	del(Zuazo::Timing::TimeUnit(33334));
	del<<src;
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
	/*
	 * 		TERMINATION
	 */

	Zuazo::end();

	std::cout << "End \n";

	return 0;
}
