#pragma once

#include <mutex>

#include "Consumer.h"
#include "Source.h"

namespace Zuazo::Stream{

template <typename T>
class CallableConsumer : public Consumer<T>{
	friend Source<T>;
public:
	CallableConsumer()=default;
	CallableConsumer(const CallableConsumer& other)=default;
	virtual ~CallableConsumer()=default;

	virtual void				setSource(Source<T>* src) override;
protected:
	virtual void				onSourceUpdate()=0;
};

/*
 * METHOD DEFINITIONS
 */

template <typename T>
inline void CallableConsumer<T>::setSource(Source<T>* src){
	std::lock_guard<std::mutex> lock(Consumer<T>::m_mutex);

	//Detach consumer from the previous source
	if(Consumer<T>::m_source){
		Consumer<T>::m_source->detach(this);
	}

	//Attach the new consumer
	Consumer<T>::m_source=src;
	if(Consumer<T>::m_source){
		Consumer<T>::m_source->attach(this);
	}
}
}
