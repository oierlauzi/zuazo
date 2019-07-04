#include "Pool.h"

namespace Zuazo::Utils {

/*
 *  DELETER
 */

template<typename T, typename... Args>
constexpr Pool<T, Args...>::Deleter::Deleter(const Pool& owner) :
    m_owner(owner)
{
}

template<typename T, typename... Args>
inline void Pool<T, Args...>::Deleter::operator()(T* ptr) const{
    m_owner.m_elements.emplace_front(ptr);
}

/*
 *  POOL
 */

template<typename T, typename... Args>
inline Pool<T, Args...>::Pool(Args&&... args) :
    m_arguments(args...)
{
}

template<typename T, typename... Args>
inline typename Pool<T, Args...>::unique_ptr Pool<T, Args...>::get() const{
    if(m_elements.empty()) {
        addElement();
    }

    auto result = unique_ptr(std::move(m_elements.front()), Deleter(*this));
    m_elements.pop_front();

    return result;
}

template<typename T, typename... Args>
template<std::size_t... Is>
T* constructElement(std::index_sequence<Is...>) const {
    return new T(std::get<Is>(m_arguments)...)
}

}