#pragma once

#include <sys/types.h>
#include <memory>

#include "../Updateable.h"
#include "Timing.h"

namespace Zuazo::Timing{

template <u_int32_t order>
class RegularUpdate :
		public virtual Updateable
{
public:
	RegularUpdate();
	RegularUpdate(const RegularUpdate& other);
	virtual ~RegularUpdate();

	virtual void						open() override;
	virtual void						close() override;
};

/*
 * METHOD DEFINITIONS
 */

template <u_int32_t order>
inline RegularUpdate<order>::RegularUpdate(){
	timings->addTiming(this);
}

template <u_int32_t order>
inline RegularUpdate<order>::RegularUpdate(const RegularUpdate& other){
	timings->addTiming(this);
}

template <u_int32_t order>
inline RegularUpdate<order>::~RegularUpdate(){
	if(timings) //Timings might have been destroyed (called end() before going out of scope)
		timings->deleteTiming(this);
}

template <u_int32_t order>
inline void RegularUpdate<order>::open(){
	Updateable::open();
	timings->addTiming(this);
}

template <u_int32_t order>
inline void RegularUpdate<order>::close(){
	timings->deleteTiming(this);
	Updateable::close();
}
}
