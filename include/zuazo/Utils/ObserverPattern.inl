#include "ObserverPattern.h"

#include <cassert>

namespace Zuazo::Utils {

inline Observer::Observer()
	: m_subject(nullptr)
{
}

inline Observer::Observer(const Observer& other)
	: m_subject(other.m_subject)
{
	attachSubject();
}


inline Observer::~Observer() {
	detachSubject();
}

inline Observer& Observer::operator=(const Observer& other) {
	detachSubject();
	m_subject = other.m_subject;
	attachSubject();
	
	return *this;
}

inline void Observer::setSubject(Subject* subject) {
	detachSubject();
	m_subject = subject;
	attachSubject();
}

inline Subject* Observer::getSubject() const {
    return m_subject;
}

inline void Observer::attachSubject() {
	if(m_subject) m_subject->m_observers.insert(this);
}

inline void Observer::detachSubject() {
	if(m_subject) m_subject->m_observers.erase(this);
}





inline Subject::Subject()
	: m_observers()
{
}

inline Subject::Subject(Subject&& other)
	: m_observers(std::move(other.m_observers))
{
	attachObservers();
}

inline Subject::~Subject() {
	detachObservers();
}

inline Subject& Subject::operator=(Subject&& other) {
	detachObservers();
    m_observers = std::move(other.m_observers);
	attachObservers();
	return *this;
}

inline const Subject::Observers& Subject::getObservers() const {
	return m_observers;
}

inline void Subject::attachObservers() {
    for(auto* observer : m_observers) {
		assert(observer);
    	observer->m_subject = this;
    }
}

inline void Subject::detachObservers() {
	for(auto* observer : m_observers) {
		assert(observer);
		assert(observer->m_subject == this);
    	observer->m_subject = nullptr;
    }
}


}