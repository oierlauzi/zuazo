#pragma once

#include <bits/types/clock_t.h>
#include <map>
#include <set>

#include "Utils/Rational.h"

namespace Zuazo::Stream{

class Timing{
public:
	int 			init();
	int 			end();

	const Rational& getInterval() const;
	Rational		getRate() const;

protected:
	Timing();
	Timing(const Rational& interval);
	Timing(const Timing& other);
	virtual ~Timing();

	void 			setInterval(const Rational& interval);
	void			setRate(const Rational& rate);

	virtual void 	update(clock_t elapsed)=0;

private:
	Rational		m_updateInterval;

	static std::map<clock_t, std::set<Timing*>> s_timings;

	static clock_t	toClk(const Rational& rat);
	static Rational toRat(clock_t clk);

	static void 	addTiming(Timing* element, clock_t interval);
	static void 	deleteTiming(Timing* element);

	static void		updateThreadFunc();
};



/*
 * INLINE FUNCTION DECLARATIONS
 */



inline const Rational& Timing::getInterval() const {
	return m_updateInterval;
}

inline Rational Timing::getRate() const {
	return 1/m_updateInterval;
}


}
