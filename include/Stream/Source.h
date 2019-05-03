#pragma once

#include <memory>
#include <set>

namespace Zuazo::Stream{

template <typename T>
class Source{
public:
	Source()=default;
	Source(const Source& other)=default;
	Source(Source&& other)=default;
	virtual ~Source()=default;

	virtual std::shared_ptr<const T>	get() const;

protected:
	void								push() const;
	void								push(const std::shared_ptr<const T>& element) const;
	void								push(std::unique_ptr<const T> element) const;

	void								reset();
private:
	mutable std::shared_ptr<const T>	m_last;
};

template <typename T>
class SourcePad :
		public Source<T>
{
public:
	using Source<T>::push;
	using Source<T>::reset;
	using Source<T>::Source;
};

/*
 * METHOD DEFINITIONS
 */

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
