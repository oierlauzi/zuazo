#pragma once

#include "Source.h"
#include "../UpdateableBase.h"

#include <sys/types.h>
#include <memory>
#include <mutex>

namespace Zuazo::Stream{

template <typename T>
class LazySource :
		public Source<T>,
		public UpdateableBase
{
public:
	using Source<T>::Source;
	virtual ~LazySource()=default;

	void								setMaxRecursion(u_int32_t rec);
	u_int32_t							getMaxRecursion() const;

	virtual void						enable() override;
	virtual void						disable() override;

	virtual std::shared_ptr<const T>	get() const override;
private:
	bool 								m_isEnabled = true;
	u_int32_t							m_maxUpdateRecursion = 1;
	mutable u_int32_t					m_updateRecursion = 0;
};


template <typename T>
void LazySource<T>::setMaxRecursion(u_int32_t rec){
	m_maxUpdateRecursion=rec;
}

template <typename T>
u_int32_t LazySource<T>::getMaxRecursion() const{
	return m_maxUpdateRecursion;
}

template <typename T>
inline void LazySource<T>::enable(){
	m_isEnabled=true;
}

template <typename T>
inline void LazySource<T>::disable(){
	m_isEnabled=false;
}


template <typename T>
inline std::shared_ptr<const T> LazySource<T>::get() const{
	if(m_updateRecursion < m_maxUpdateRecursion && m_isEnabled){
		m_updateRecursion++;

		if(m_updateRecursion == 1){
			//To avoid deadlocks, lock the mutex only in the first update
	
			update();
		}else{
			update();
		}

		m_updateRecursion--;
	}

	return Stream::Source<T>::get();
}

template <typename T>
class LazySourcePad :
		public LazySource<T>
{
public:
	LazySourcePad(const UpdateableBase& owner);

	void update() const override;
	using Source<T>::push;
	using Source<T>::reset;
private:
	const UpdateableBase& m_owner;
};

/*
 * METHOD DEFINITIONS
 */

template <typename T>
inline LazySourcePad<T>::LazySourcePad(const UpdateableBase& owner) :
	m_owner(owner)
{
}

template <typename T>
inline void LazySourcePad<T>::update() const{
	m_owner.update();
}

}
