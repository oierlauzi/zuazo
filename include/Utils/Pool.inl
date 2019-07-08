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

}
