#pragma once

#include "Consumer.h"

#include <memory>
#include <set>

namespace Zuazo::Stream{

template <typename T>
class Consumer;

template <typename T>
class Source{
	friend Consumer<T>;
public:
	Source()=default;
	Source(const Source& other)=delete;
	Source(Source&& other);
	virtual ~Source();

	virtual std::shared_ptr<const T>	get() const;

protected:
	void								push() const;
	void								push(const std::shared_ptr<const T>& element) const;
	void								push(std::unique_ptr<const T> element) const;

	void								reset();
private:
	mutable std::shared_ptr<const T>	m_last;

	mutable std::set<Consumer<T>*>		m_consumers;
};


template <typename T, typename Q>
class SourcePad :
		public Source<T>
{
	friend Q;
};

/*
 * METHOD DEFINITIONS
 */

template <typename T>
Source<T>::~Source(){
	while(m_consumers.size())
		(*m_consumers.begin())->setSource(nullptr);
}

template <typename T>
Source<T>::Source(Source&& other){
	while(other.m_consumers.size()){
		(*m_consumers.begin())->setSource(this);
	}
	m_last=other.m_last;
}

template <typename T>
inline std::shared_ptr<const T> Source<T>::get() const{
	return m_last;
}

template <typename T>
inline void	Source<T>::push() const{
	m_last=std::shared_ptr<const T>();
}

template <typename T>
inline void	Source<T>::push(const std::shared_ptr<const T>& element) const{
	m_last=element;
}

template <typename T>
inline void	Source<T>::push(std::unique_ptr<const T> element) const{
	m_last=std::move(element);
}

template <typename T>
inline void	Source<T>::reset(){
	push();
}
}
