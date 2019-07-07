#include "Pool.h"

namespace Zuazo::Utils {

/*
 *  DELETER
 */

template<typename T, typename... Args>
constexpr Pool<T, Args...>::Deleter::Deleter() :
	m_owner(nullptr)
{
}

template<typename T, typename... Args>
constexpr Pool<T, Args...>::Deleter::Deleter(const Arguments& args, const Pool& owner) :
	m_args(args),
	m_owner(&owner)
{
}

template<typename T, typename... Args>
inline void Pool<T, Args...>::Deleter::operator()(T* ptr) const{
	std::unique_lock<std::mutex> lock(m_owner->m_mutex);
	m_owner->m_elements[m_args].emplace_front(ptr);
}

/*
 *  POOL
 */

template<typename T, typename... Args>
inline typename Pool<T, Args...>::unique_ptr Pool<T, Args...>::get(Args&&... args) const{
	const Arguments argTuple(args...);

	std::unique_lock<std::mutex> lock(m_mutex);

	auto ite = m_elements.find(argTuple);
	if(ite == m_elements.end()){
		//This kind element does not exist. I needs to be constructed
		return unique_ptr(
			new T(std::forward<Args>(args)...),
			Deleter(argTuple, *this)
		);	
	} else {
		//This kind element exists. Take it out form the pool
		auto result = unique_ptr(
			ite->second.front().release(),
			Deleter(argTuple, *this)
		);

		ite->second.pop_front();

		if(ite->second.empty()) {
			//No elements left
			m_elements.erase(ite);
		}

		return result;
	}
}

/*
 * HASHER
 */

template<typename T, typename... Args>
constexpr size_t Pool<T,  Args...>::Hasher::operator()(const Arguments& a) const{
	constexpr size_t tupleSize = std::tuple_size<Arguments>::value;

	if constexpr (tupleSize){
		return hasher<tupleSize - 1>(a);
	} else {
		return 0;
	}
}

template<typename T, typename... Args>
template<size_t idx>
constexpr size_t Pool<T,  Args...>::Hasher::hasher(const Arguments& a) {
	using Element = typename std::tuple_element<idx, Arguments>::type;
	const size_t hash = std::hash<Element>()(std::get<idx>(a)); //Current element's hash

	if constexpr (idx){
		const size_t prevHash = hasher<idx - 1>(a); //Previous element's hash
		return prevHash ^ (hash + 0x9e3779b9 + (prevHash << 6) + (prevHash >> 2));
	} else {
		return hash;
	}
}

}
