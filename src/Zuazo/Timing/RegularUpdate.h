#pragma once

#include <memory>

#include "../Updateable.h"
#include "TimingTable.h"
#include "UpdateOrder.h"

namespace Zuazo::Timing{

extern std::unique_ptr<TimingTable> timings;

template <UpdateOrder TPriority>
class RegularUpdate : public Updateable{
	friend TimingTable;
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

template <UpdateOrder TPriority>
inline RegularUpdate<TPriority>::RegularUpdate(){
	timings->addTiming(this);
}

template <UpdateOrder TPriority>
inline RegularUpdate<TPriority>::RegularUpdate(const RegularUpdate& other){
	timings->addTiming(this);
}

template <UpdateOrder TPriority>
inline RegularUpdate<TPriority>::~RegularUpdate(){
	if(timings) //Timings might have been destroyed (called end() before going out of scope)
		timings->deleteTiming(this);
}

template <UpdateOrder TPriority>
inline void RegularUpdate<TPriority>::open(){
	Updateable::open();
	timings->addTiming(this);
}

template <UpdateOrder TPriority>
inline void RegularUpdate<TPriority>::close(){
	timings->deleteTiming(this);
	Updateable::close();
}
}
