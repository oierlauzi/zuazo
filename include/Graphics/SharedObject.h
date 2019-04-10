#pragma once

#include "Context.h"

#include <memory>

namespace Zuazo::Graphics{

template<typename T>
class SharedObject{
public:
	SharedObject();
	SharedObject(const SharedObject& other);
	virtual ~SharedObject();

	T&	get();
private:
	struct Container{
		T			obj;
		u_int32_t 	useCount=0;
	};

	static std::map<const Context*, Container>	s_objs;

	const Context*	m_ctx;
	Container&		m_obj;
};

template<typename T>
inline std::map<const Context*, typename SharedObject<T>::Container> SharedObject<T>::s_objs;

template<typename T>
inline SharedObject<T>::SharedObject() :
	m_ctx(Context::getCurrentCtx()),
	m_obj(s_objs[m_ctx])
{
	++m_obj.useCount;
}

template<typename T>
inline SharedObject<T>::SharedObject(const SharedObject& other) :
	m_ctx(other.m_ctx),
	m_obj(other.m_obj)
{
	++m_obj.useCount;
}

template<typename T>
inline SharedObject<T>::~SharedObject(){
	if(--m_obj.useCount == 0){
		s_objs.erase(m_ctx);
	}
}

template<typename T>
inline T&	SharedObject<T>::get(){
	return m_obj.obj;
}

}
