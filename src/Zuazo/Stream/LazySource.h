#pragma once

#include <sys/types.h>
#include <memory>
#include <mutex>

#include "../Timing/UpdateableBase.h"
#include "Source.h"

namespace Zuazo::Stream{

template <typename T>
class LazySource :
		public Source<T>,
		public Timing::UpdateableBase
{
public:
	using Source<T>::Source;
	virtual ~LazySource()=default;

	void								setMaxRecursion(u_int32_t rec);
	u_int32_t							getMaxRecursion() const;

	virtual void						enable() override;
	virtual void						disable() override;
protected:
	bool 								m_isEnabled=true;
	u_int32_t							m_maxUpdateRecursion=1;
	mutable u_int32_t					m_updateRecursion=0;

	virtual std::shared_ptr<const T>	get() const override;
};


template <typename T>
void LazySource<T>::setMaxRecursion(u_int32_t rec){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_maxUpdateRecursion=rec;
}

template <typename T>
u_int32_t LazySource<T>::getMaxRecursion() const{
	return m_maxUpdateRecursion;
}

template <typename T>
inline void LazySource<T>::enable(){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_isEnabled=true;
}

template <typename T>
inline void LazySource<T>::disable(){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_isEnabled=false;
}


template <typename T>
inline std::shared_ptr<const T> LazySource<T>::get() const{
	if(m_updateRecursion < m_maxUpdateRecursion && m_isEnabled){
		m_updateRecursion++;

		if(m_updateRecursion == 1){
			//To avoid deadlocks, lock the mutex only in the first update
			std::lock_guard<std::mutex> lock(m_updateMutex);
			update();
		}else{
			update();
		}

		m_updateRecursion--;
	}

	return Stream::Source<T>::get();
}

template <typename T, typename Q>
class LazySourcePad :
		public LazySource<T>
{
	friend Q;
public:
	LazySourcePad(const Timing::UpdateableBase& owner);

	void update() const override;
protected:
	using LazySource<T>::m_updateRecursion;
private:
	const Timing::UpdateableBase& m_owner;
};
/*
 * METHOD DEFINITIONS
 */

template <typename T, typename Q>
inline LazySourcePad<T, Q>::LazySourcePad(const Timing::UpdateableBase& owner) :
	m_owner(owner)
{
}

template <typename T, typename Q>
inline void LazySourcePad<T, Q>::update() const{
	if(m_updateRecursion == 1){
		std::lock_guard<const Timing::UpdateableBase> lock(m_owner);
		m_owner.update();
	}else
		m_owner.update();
}

}
