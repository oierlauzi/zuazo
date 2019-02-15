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
	virtual std::shared_ptr<const T> 	get(Timing::TimePoint* ts) const override;

	virtual void						open() override;
	virtual void						close() override;
protected:
	void								perform() override;
private:
	bool								m_updateInProgress;
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
	const_cast<LazySource<T>*>(this)->perform();
	return Source<T>::get();
}

template <typename T>
inline std::shared_ptr<const T>	LazySource<T>::get(Timing::TimePoint* ts) const{
	const_cast<LazySource<T>*>(this)->perform();
	return Source<T>::get(ts);
}

template <typename T>
void LazySource<T>::open(){
	Source<T>::open();
}

template <typename T>
void LazySource<T>::close(){
	Source<T>::close();
}

template <typename T>
inline void LazySource<T>::perform(){
	//Only update if it is not being updated (to avoid mutex deadlocks and endless loops)
	if(!m_updateInProgress){
		m_updateInProgress=true;
		Updateable::perform();
		m_updateInProgress=false;
	}
}
}
