#pragma once

namespace Zuazo::Graphics{

template<typename T>
class UniqueBinding{
public:
	UniqueBinding(const T& bindable);
	UniqueBinding(const UniqueBinding& other)=delete;
	~UniqueBinding();
private:
	const T&	m_target;
};

template<typename T>
inline UniqueBinding<T>::UniqueBinding(const T& bindable) : m_target(bindable){
	m_target.bind();
}

template<typename T>
inline UniqueBinding<T>::~UniqueBinding(){
	m_target.unbind();
}

}
