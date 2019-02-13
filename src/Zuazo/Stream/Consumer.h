#pragma once

#include <cstddef>
#include <memory>
#include <mutex>

#include "Source.h"

namespace Zuazo::Stream{

template <typename T>
class Source;

template <typename T>
class Consumer{
	friend Source<T>;
public:
	Consumer();
	Consumer(const Consumer<T>& other)=default;
	virtual ~Consumer();

	void						setSource(const Source<T>* src);
	Consumer<T>&				operator<<(const Source<T>& src);
	Consumer<T>&				operator<<(std::nullptr_t ptr);
	virtual bool 				isActive() const { return true; }
protected:
	std::shared_ptr<const T>	get() const;
	std::shared_ptr<const T>	get(Timing::TimePoint* ts) const;

private:
	mutable std::mutex			m_mutex;

	const Source<T>*			m_source;
};

/************************
 * FUNCTION DEFINITIONS	*
 ************************/
/*
 * CONSTRUCTOR AND DESTRUCTOR
 */

template <typename T>
inline Consumer<T>::Consumer(){
	m_source=nullptr;
}

template <typename T>
inline Consumer<T>::~Consumer(){
	setSource(nullptr);
}

/*
 * SOURCE MODIFERS
 */

template <typename T>
inline void Consumer<T>::setSource(const Source<T>* src){
	std::lock_guard<std::mutex> lock(m_mutex);

	//Detach consumer from the previous source
	if(m_source)
		m_source->detach(this);

	//Attach the new consumer
	m_source=src;
	if(m_source)
		m_source->attach(this);
}

template <typename T>
inline Consumer<T>& Consumer<T>::operator<<(const Source<T>& src){
	setSource(&src);
	return *this;
}

template <typename T>
inline Consumer<T>& Consumer<T>::operator<<(std::nullptr_t ptr){
	setSource(nullptr);
	return *this;
}

template <typename T>
inline std::shared_ptr<const T>	Consumer<T>::get() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	if(m_source)
		return m_source->get();
	else
		return std::shared_ptr<const T>(); //No source, return an empty ptr
}

template <typename T>
inline std::shared_ptr<const T>	Consumer<T>::get(Timing::TimePoint* ts) const{
	std::lock_guard<std::mutex> lock(m_mutex);
	if(m_source)
		return m_source->get(ts);
	else
		return std::shared_ptr<const T>(); //No source, return an empty ptr
}

} /* namespace Zuazo */
