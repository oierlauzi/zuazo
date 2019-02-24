#pragma once

#include <cstddef>
#include <memory>
#include <mutex>

#include "Source.h"

namespace Zuazo::Stream{

template <typename T>
class Source;

template <typename T>
class Consumer :
	public virtual Updateable
{
	friend Source<T>;
public:
	Consumer();
	Consumer(const Consumer<T>& other)=default;
	virtual ~Consumer();

	void						setSource(const Source<T>* src);
	Consumer<T>&				operator<<(const Source<T>& src);
	Consumer<T>&				operator<<(std::nullptr_t ptr);

	virtual void				open() override;
	virtual void				close() override;
protected:
	std::shared_ptr<const T>	get() const;
	bool						hasChanged() const;

private:
	const Source<T>*			m_source;

	mutable std::weak_ptr<const T>	m_lastFrame;

	std::shared_ptr<const T>	_get() const;
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
	std::lock_guard<std::mutex> lock(m_updateMutex);

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
void Consumer<T>::open(){
	Updateable::open();
}

template <typename T>
void Consumer<T>::close(){
	Updateable::close();
}

template <typename T>
inline std::shared_ptr<const T>	Consumer<T>::get() const {
	std::shared_ptr<const T> frame=_get();
	m_lastFrame=frame;
	return frame;
}

template <typename T>
inline bool Consumer<T>::hasChanged() const {
	return m_lastFrame == _get();
}

template <typename T>
inline std::shared_ptr<const T>	Consumer<T>::_get() const {
	if(m_source)
		return m_source->get();
	else
		return std::shared_ptr<const T>(); //No source, return an empty ptr
}

} /* namespace Zuazo */
