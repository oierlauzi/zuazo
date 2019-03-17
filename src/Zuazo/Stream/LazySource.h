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

protected:
	virtual void						enable() override;
	virtual void						disable() override;

	virtual std::shared_ptr<const T>	get() const override;
private:
	u_int32_t							m_maxUpdateRecursion=1;
	mutable u_int32_t					m_updateRecursion=0;
};

template <typename T, typename Q>
class LazySourcePad :
		public Source<T>
{
	friend Q;
public:
	void update() const override;

private:
	using Source<T>::m_updateRecursion;
	LazySourcePad(const Timing::UpdateableBase& owner);

	const Timing::UpdateableBase& m_owner;
};
/*
 * METHOD DEFINITIONS
 */

template <typename T>
inline std::shared_ptr<const T> LazySource<T>::get() const{
	if(m_updateRecursion < m_maxUpdateRecursion){
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
inline LazySourcePad<T, Q>::LazySourcePad(const Timing::UpdateableBase& owner) :
	m_owner(owner)
{
}

template <typename T, typename Q>
inline void LazySourcePad<T, Q>::update() const{
	if(m_updateRecursion == 1){
		std::lock_guard<Timing::UpdateableBase> lock(m_owner);
		m_owner.update();
	}else
		m_owner.update();
}

}
