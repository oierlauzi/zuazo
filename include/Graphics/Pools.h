#pragma once

#include <stddef.h>
#include <sys/types.h>
#include <map>
#include <memory>
#include <mutex>
#include <queue>

namespace Zuazo::Graphics{

/**
 * @brief A template for creating pools of GL resources
 */
template<typename T>
class Pool{
public:
	constexpr Pool()=default;
	Pool(const Pool& other)=delete;
	~Pool()=default;

	std::unique_ptr<T>						pop();
	void									push(std::unique_ptr<T> el);

	size_t									size() const;
private:
	static constexpr u_int32_t				THRESHOLD=4;

	mutable std::queue<std::unique_ptr<T>>	m_elements;
	mutable std::mutex						m_mutex;
};

template<typename T>
inline std::unique_ptr<T> Pool<T>::pop(){
	std::unique_ptr<T> result;
	std::lock_guard<std::mutex> lock(m_mutex);

	if(m_elements.size()){
		//Requested element.exists
		result=std::move(m_elements.front());
		m_elements.pop();
	}

	return result;
}

template<typename T>
inline void Pool<T>::push(std::unique_ptr<T> el){
	std::lock_guard<std::mutex> lock(m_mutex);

	if(m_elements.size() <= THRESHOLD)
		m_elements.push(std::move(el));
}

template<typename T>
inline size_t Pool<T>::size() const{
	return m_elements.size();
}



/**
 * @brief A template for creating pools of GL resources, ordered by a key
 */
template<typename Q, typename T>
class MultiPool{
public:
	constexpr MultiPool()=default;
	MultiPool(const MultiPool& other)=delete;
	~MultiPool()=default;

	std::unique_ptr<T>						pop(const Q& ref);
	void									push(const Q& ref, std::unique_ptr<T> el);

	size_t									size() const;
private:
	static constexpr u_int32_t				THRESHOLD=4;

	mutable std::map<Q, std::queue<std::unique_ptr<T>>>	m_elements;
	mutable std::mutex						m_mutex;
};

template<typename Q, typename T>
inline std::unique_ptr<T> MultiPool<Q, T>::pop(const Q& ref){
	std::unique_ptr<T> result;
	std::lock_guard<std::mutex> lock(m_mutex);

	auto ite=m_elements.find(ref);

	if(ite != m_elements.end()){
		//Element exists
		result=std::move(ite->second.front());
		ite->second.pop();

		if(ite->second.size() == 0){
			//The pool is empty. Delete it
			m_elements.erase(ite);
		}
	}

	return result;
}

template<typename Q, typename T>
inline void MultiPool<Q, T>::push(const Q& ref, std::unique_ptr<T> el){
	std::lock_guard<std::mutex> lock(m_mutex);

	auto ite=m_elements.find(ref);

	if(ite != m_elements.end()){
		if(ite->second.size() <= THRESHOLD){
			ite->second.push(std::move(el));
		}
	}else{
		m_elements[ref].push(std::move(el));
	}
}

template<typename Q, typename T>
inline size_t MultiPool<Q, T>::size() const{
	return m_elements.size();
}
}
