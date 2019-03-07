#pragma once

#include <memory>

#include "Source.h"

namespace Zuazo::Stream{

template <typename T>
class LazySource :
		public Source<T>
{
public:
	using Source<T>::Source;
	virtual ~LazySource()=default;

	virtual std::shared_ptr<const T>		get() const override;

	using Source<T>::open;
	using Source<T>::close;
private:
	mutable bool							m_updateInProgress=false;
};


/*
 * METHOD DEFINITIONS
 */

template <typename T>
inline std::shared_ptr<const T> LazySource<T>::get() const{
	//Only update if it is not being updated (to avoid mutex deadlocks and endless loops)
	if(!m_updateInProgress){
		m_updateInProgress=true;
		Updateables::Updateable::perform();
		m_updateInProgress=false;
	}
	return Source<T>::get();
}

}
