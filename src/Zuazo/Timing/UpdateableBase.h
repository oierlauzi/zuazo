#pragma once

#include <memory>
#include <mutex>

namespace Zuazo::Timing{

class UpdateableBase{
public:
	UpdateableBase()=default;
	UpdateableBase(const UpdateableBase& other)=default;
	virtual ~UpdateableBase()=default;

	bool								try_lock() const;
	void								lock() const;
	void								unlock() const;

	virtual void						update() const=0;

	virtual void 						enable()=0;
	virtual void 						disable()=0;
protected:
	mutable std::mutex					m_updateMutex;
};

/*
 * INLINE METHOD DEFINITIONS
 */

inline bool UpdateableBase::try_lock() const{
	return m_updateMutex.try_lock();
}

inline void UpdateableBase::lock() const{
	m_updateMutex.lock();
}


inline void UpdateableBase::unlock() const{
	m_updateMutex.unlock();
}

}
