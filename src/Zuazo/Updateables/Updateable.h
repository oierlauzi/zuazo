#pragma once

#include <memory>
#include <mutex>

namespace Zuazo{
class Timing;
}

namespace Zuazo::Updateables{
/*
 * Class which defines a periodical event
 */
class Updateable{
	friend Timing;
public:
	/*
	 * An update event's callback function
	 */
	class Callback{
	public:
		virtual void						update(const Updateable *) const=0;
	};

	Updateable();
	Updateable(const Updateable& other);
	virtual ~Updateable()=default;

	void								setBeforeUpdateCallback(const std::shared_ptr<Callback>& cbk);
	void								setAfterUpdateCallback(const std::shared_ptr<Callback>& cbk);

	void								lock() const;
	void								unlock() const;

	virtual void						open();
	void								asyncOpen();
	virtual void						close();
	void								asyncClose(); //TODO
	bool								isOpen() const;
protected:
	mutable std::mutex					m_updateMutex;

	virtual void						update() const=0;
	void								perform() const;
private:
	mutable std::mutex					m_mutex;

	bool								m_isOpen;

	std::shared_ptr<Callback>			m_beforeCbk;
	std::shared_ptr<Callback>			m_afterCbk;
};

/*
 * INLINE METHOD DEFINITIONS
 */

inline void Updateable::setBeforeUpdateCallback(const std::shared_ptr<Callback>& cbk){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_beforeCbk=cbk;
}


inline void Updateable::setAfterUpdateCallback(const std::shared_ptr<Callback>& cbk){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_afterCbk=cbk;
}


inline void Updateable::lock() const{
	m_mutex.lock();
}


inline void Updateable::unlock() const{
	m_mutex.unlock();
}


inline void Updateable::open(){
	m_isOpen=true;
}


inline void Updateable::close(){
	m_isOpen=false;
}


inline bool Updateable::isOpen() const{
	return m_isOpen;
}

}
