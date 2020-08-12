#pragma once

#include <unordered_set>

namespace Zuazo::Utils {

class Observer;
class Subject;

class Observer {
	friend Subject;
public:
	Observer();
	Observer(const Observer& other);
	~Observer();

	Observer&			operator=(const Observer& other);

	void				setSubject(Subject* subject);
	Subject*			getSubject() const;

private:
	Subject*			m_subject;

	void				attachSubject();
	void				detachSubject();

};

class Subject {
	friend Observer;
public:
	using Observers = std::unordered_set<Observer*>;

	Subject();
	Subject(Subject&& other);
	~Subject();

	Subject&			operator=(Subject&& other);

	const Observers&	getObservers() const;

private:
	Observers			m_observers;

	void				attachObservers();
	void				detachObservers();

};

}

#include "ObserverPattern.inl"
