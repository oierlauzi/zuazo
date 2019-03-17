#pragma once

#include <ctpl/ctpl.h>
#include <future>
#include <memory>
#include <mutex>

namespace Zuazo::Timing{
class Timings;
}

namespace Zuazo{
/*
 * Class which defines a periodical event
 */
class ZuazoBase{
public:
	ZuazoBase()=default;
	ZuazoBase(const ZuazoBase& other)=default;
	virtual ~ZuazoBase()=default;

	virtual void						open();
	void								asyncOpen();
	virtual void						close();
	void								asyncClose();
	bool								isOpen() const;
private:
	mutable std::mutex					m_mutex;

	bool								m_isOpen=false;

	static ctpl::thread_pool			s_asyncOpenCloseThreads;
};

/*
 * INLINE METHOD DEFINITIONS
 */


inline void ZuazoBase::open(){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_isOpen=true;
}

inline void ZuazoBase::asyncOpen(){
	//s_asyncOpenCloseThreads.push(&ZuazoBase::open, this); //TODO
}

inline void ZuazoBase::close(){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_isOpen=false;
}

inline void ZuazoBase::asyncClose(){
	//s_asyncOpenCloseThreads.push(&ZuazoBase::close, this); //TODO
}

inline bool ZuazoBase::isOpen() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_isOpen;
}

}
