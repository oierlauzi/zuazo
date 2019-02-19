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
	RegularUpdate()=default;
	RegularUpdate(const RegularUpdate& other)=default;
	virtual ~RegularUpdate()=default;

	virtual void						open() override;
	virtual void						close() override;
};

/*
 * METHOD DEFINITIONS
 */

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
