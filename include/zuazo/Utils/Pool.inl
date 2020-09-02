#include "Pool.h"

#include "Functions.h"

namespace Zuazo::Utils {

/*
 * Pool
 */

template <typename T, typename Alloc>
inline Pool<T, Alloc>::Pool(Allocator alloc)
	: m_data(Spares(), std::move(alloc))
{
}


template <typename T, typename Alloc>
inline std::shared_ptr<T> Pool<T, Alloc>::acquire() {
	for(const auto& el : spares()) {
		assert(el.use_count() > 0);
		if(el.use_count() == 1) return el;
	}

	spares().push_back(allocator()());
	assert(spares().back().use_count() == 1);
	return spares().back();
}

template <typename T, typename Alloc>
inline void Pool<T, Alloc>::clear() {
	spares().clear();
}



template <typename T, typename Alloc>
inline typename Pool<T, Alloc>::Spares& Pool<T, Alloc>::spares() {
	return std::get<m_spares>(m_data);
}

template <typename T, typename Alloc>
inline const typename Pool<T, Alloc>::Spares& Pool<T, Alloc>::spares() const {
	return std::get<m_spares>(m_data);
}


template <typename T, typename Alloc>
inline typename Pool<T, Alloc>::Allocator& Pool<T, Alloc>::allocator() {
	return std::get<m_allocator>(m_data);
}

template <typename T, typename Alloc>
inline const typename Pool<T, Alloc>::Allocator& Pool<T, Alloc>::allocator() const {
	return std::get<m_allocator>(m_data);
}


}