#pragma once

#include <chrono>
#include <memory>

#include "../Timing.h"

namespace Zuazo::Stream{

template <typename T>
struct StreamElement{
	std::unique_ptr<T> 	element;
	time_unit 			ts;

	StreamElement(std::unique_ptr<T>& el){
		element=std::move(el);
		ts=Timing::getTimingTable().getCurrentTimeStamp();
	}

	StreamElement(const StreamElement<T>& other)=delete;

	time_unit age(){
		return Timing::getTimingTable().getCurrentTimeStamp() - ts;
	}
};

}
