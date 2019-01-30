#pragma once

#include <mutex>

#include "Utils/Rational.h"

namespace Zuazo{

class Timing;

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
		virtual void						update()=0;
	};

	Updateable();
	Updateable(const Utils::Rational& rate);
	Updateable(const Updateable& other);
	virtual ~Updateable();
	void								setBeforeUpdateCallback(Callback * cbk);
	void								setAfterUpdateCallback(Callback * cbk);

	const Utils::Rational& 				getInterval() const;
	Utils::Rational						getRate() const;

	bool								isOpen() const;
protected:
	mutable std::mutex					m_mutex;

	void 								setInterval(const Utils::Rational& interval);
	void								setRate(const Utils::Rational& rate);

	virtual void						open();
	virtual void						close();

	virtual void						update()=0;
	virtual void						perform();
private:
	Utils::Rational						m_updateInterval;
	bool								m_isOpen;

	Callback *							m_beforeCbk;
	Callback *							m_afterCbk;

	std::mutex							m_cbkMutex;
};

/*
 * INLINE METHOD DEFINITIONS
 */

inline const Utils::Rational& Updateable::getInterval() const {
	return m_updateInterval;
}

inline Utils::Rational Updateable::getRate() const {
	return 1/m_updateInterval;
}

inline bool Updateable::isOpen() const{
	return m_isOpen;
}








}
