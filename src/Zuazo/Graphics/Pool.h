#pragma once

#include <map>
#include <queue>
#include <memory>
#include <mutex>

#include "GL/Context.h"

namespace Zuazo::Graphics{

/**
 * @brief A template for creating pools of GL resources
 */
template<typename T, int maxElements=32>
class Pool{
public:
	constexpr Pool()=default;
	Pool(const Pool& other)=delete;
	~Pool()=default;

	std::unique_ptr<T>					pop();
	void								push(std::unique_ptr<T> el);

	size_t								size() const;
	void								decrement(u_int32_t no=1);
private:
	std::queue<std::unique_ptr<T>>		m_elements;
	std::mutex							m_mutex;
};

/**
 * @brief A template for creating pools of GL resources, ordered by a key
 */
template<typename Q, typename T, int maxElements=32>
class MultiPool{
public:
	constexpr MultiPool()=default;
	MultiPool(const MultiPool& other)=delete;
	~MultiPool()=default;

	std::unique_ptr<T>					pop(const Q& ref);
	void								push(const Q& ref, std::unique_ptr<T> el);

	size_t								size() const;
	void								decrement(u_int32_t no=1);
private:
	std::map<Q, Pool<T, maxElements>>	m_elements;
	std::mutex							m_mutex;
};

/*
 * POOL METHOD DEFINITIONS
 */

template<typename T, int maxElements>
inline std::unique_ptr<T> Pool<T, maxElements>::pop(){
	std::unique_ptr<T> result;
	std::lock_guard<std::mutex> lock(m_mutex);

	if(m_elements.size()){
		//Requested element.exists
		result=std::move(m_elements.front());
		m_elements.pop();
	}else{
		//Requested element does not exist -> construct it
		GL::UniqueContext ctx(GL::Context::mainCtx);
		result=std::unique_ptr<T>(new T());
	}

	return result;
}

template<typename T, int maxElements>
inline void Pool<T, maxElements>::push(std::unique_ptr<T> el){
	std::lock_guard<std::mutex> lock(m_mutex);

	if(m_elements.size() < maxElements){
		//There is room for another element
		m_elements.push(std::move(el));
	}else{
		//There is no room for another element
		el.reset();
	}
}

template<typename T, int maxElements>
inline size_t Pool<T, maxElements>::size() const{
	return m_elements.size();
}

template<typename T, int maxElements>
inline void Pool<T, maxElements>::decrement(u_int32_t no){
	std::lock_guard<std::mutex> lock(m_mutex);
	for(u_int32_t i=0; i<no && m_elements.size(); i++){
		m_elements.pop();
	}
}

/*
 * MULTI-POOL METHOD DEFINITIONS
 */

template<typename Q, typename T, int maxElements>
inline std::unique_ptr<T> MultiPool<Q, T, maxElements>::pop(const Q& ref){
	std::unique_ptr<T> result;
	std::lock_guard<std::mutex> lock(m_mutex);

	if(m_elements.find(ref) != m_elements.end()){
		//Element exists
		result=m_elements[ref].pop();

		if(m_elements[ref].size() == 0){
			//The pool is empty. Delete it
			m_elements.erase(ref);
		}
	}else{
		//Requested element does not exist, construct it
		GL::UniqueContext ctx(GL::Context::mainCtx);
		result=std::unique_ptr<T>(new T());
	}

	return result;
}

template<typename Q, typename T, int maxElements>
inline void MultiPool<Q, T, maxElements>::push(const Q& ref, std::unique_ptr<T> el){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_elements[ref].push(std::move(el));
}

template<typename Q, typename T, int maxElements>
inline size_t MultiPool<Q, T, maxElements>::size() const{
	return m_elements.size();
}

template<typename Q, typename T, int maxElements>
inline void MultiPool<Q, T, maxElements>::decrement(u_int32_t no){
	std::lock_guard<std::mutex> lock(m_mutex);
	auto ite=m_elements.begin();

	while(ite!=m_elements.end()){
		//Decrement the pool
		ite->decrement(no);

		if(ite->size()){
			//The pool is empty. Delete it
			ite=m_elements.erase(ite);
		}else{
			ite++;
		}
	}
}

}
