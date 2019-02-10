#pragma once

#include <map>
#include <queue>
#include <memory>

#include "GL/Context.h"

namespace Zuazo::Graphics{

/**
 * @brief A pool for GL related classes ordered by a key (usually resolution)
 * to avoid expensive construction/destruction and reallocations
 */
template<typename T, typename Q>
class Pool{
public:
	constexpr Pool(u_int32_t elements=32);
	Pool(const Pool& other)=delete;
	~Pool();

	std::unique_ptr<Q>								pop(const T& ref);
	void											push(const T& ref, std::unique_ptr<Q>& el);
private:
	u_int32_t										m_nMaxElements;
	std::map<T, std::queue<std::unique_ptr<Q>>>		m_elements;
};

/*
 * METHOD DEFINITIONS
 */

template<typename T, typename Q>
constexpr Pool<T, Q>::Pool(u_int32_t elements=32) : m_nMaxElements(elements){

}

template<typename T, typename Q>
constexpr std::unique_ptr<Q> Pool<T, Q>::pop(const T& ref){
	std::unique_ptr<Q> result;

	if(m_elements[ref].size()){
		//Requested element.exists
		result=std::move(m_elements[ref].first());
		m_elements[ref].pop();
	}else{
		//Requested element does not exist -> construct it
		GL::UniqueContext ctx(GL::Context::mainCtx);
		result=std::unique_ptr<Q>(new Q);
	}

	return result;
}

template<typename T, typename Q>
constexpr void Pool<T, Q>::push(const T& ref, std::unique_ptr<Q>& el){
	if(m_elements[ref].size() < m_nMaxElements){
		//There is room for another element
		m_elements[ref].push(std::move(el));
	}else{
		//There is no room for another element
		el.reset();
	}
}

}
