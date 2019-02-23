#pragma once

#include <memory>

#include "../Timing/TimePoint.h"
#include "../Updateable.h"
#include "Source.h"

namespace Zuazo::Stream{

template <typename T>
class LazySource :
		public Source<T>
{
public:
	LazySource();
	LazySource(const LazySource& other)=delete;
	virtual ~LazySource();

	virtual std::shared_ptr<const T>	get() const override;

	virtual void						open() override;
	virtual void						close() override;
private:
	mutable bool						m_updateInProgress;
};

/*
 * METHOD DEFINITIONS
 */

template <typename T>
inline LazySource<T>::LazySource() : Source<T>::Source(){
	m_updateInProgress=false;
}

template <typename T>
inline LazySource<T>::~LazySource(){

}


template <typename T>
inline std::shared_ptr<const T>	LazySource<T>::get() const{
	//Only update if it is not being updated (to avoid mutex deadlocks and endless loops)
	if(!m_updateInProgress){
		m_updateInProgress=true;
		Updateable::perform();
		m_updateInProgress=false;
	}
	return Source<T>::get();
}

template <typename T>
void LazySource<T>::open(){
	Source<T>::open();
}

template <typename T>
void LazySource<T>::close(){
	Source<T>::close();
}
}
