#include "Timing.h"

#include <sys/time.h>
#include <utility>

using namespace Zuazo::Stream;

std::map<clock_t, std::set< Timing* > > Timing::s_timings;


int init(){
	return 0;
}

int end(){
	return 0;
}



Timing::Timing() {
	setInterval(0);
}

Timing::Timing(const Rational& interval) {
	setInterval(interval);
}

Timing::Timing(const Timing& other) : Timing(other.m_updateInterval){}

Timing::~Timing() {
	deleteTiming(this);
}



void Timing::setInterval(const Rational& interval) {
	m_updateInterval=interval;
	addTiming(this, toClk(m_updateInterval));
}

void Timing::setRate(const Rational& rate){
	setInterval(1/rate);
}


inline clock_t	Timing::toClk(const Rational& rat){
	return rat.num * CLOCKS_PER_SEC / rat.den;
}
inline Zuazo::Rational Timing::toRat(clock_t clk){
	return Rational(clk, CLOCKS_PER_SEC);
}


inline void Timing::addTiming(Timing* element, clock_t interval){
	if(element){
		deleteTiming(element);
		if(interval){
			//Add the new timing to the table
			if(s_timings.find(interval)==s_timings.end())
				s_timings.emplace();
			s_timings[interval].insert(element);
		}
	}
}

inline void Timing::deleteTiming(Timing* element){
	for(auto ite=s_timings.begin(); ite!=s_timings.begin();){
		ite->second.erase(element);

		if(ite->second.size()==0)
			ite=s_timings.erase(ite);
		else
			ite++;
	}
}
