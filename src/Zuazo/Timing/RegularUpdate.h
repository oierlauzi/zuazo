#pragma once

#include <sys/types.h>
#include <memory>

#include "Timings.h"
#include "UpdateableBase.h"

namespace Zuazo{
extern std::unique_ptr<Timing::Timings> timings;
}

namespace Zuazo::Timing{

template <u_int32_t order>
class RegularUpdate :
		public virtual UpdateableBase
{
public:
	RegularUpdate()=default;
	RegularUpdate(const RegularUpdate& other)=default;
	virtual ~RegularUpdate();

	virtual void enable() override;
	virtual void disable() override;
};

/*
 * METHOD DEFINITIONS
 */

template <u_int32_t order>
inline RegularUpdate<order>::~RegularUpdate(){
	disable();
}

template <u_int32_t order>
inline void RegularUpdate<order>::enable(){
	timings->addTiming(this);
}

template <u_int32_t order>
inline void RegularUpdate<order>::disable(){
	if(timings)
		timings->deleteTiming(this);
}
}
