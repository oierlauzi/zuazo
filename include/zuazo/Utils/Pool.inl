#include "Pool.h"

namespace Zuazo::Utils {

/*
 * Pool
 */

template <typename T, typename Alloc, typename Del>
inline Pool<T, Alloc, Del>::Pool(Allocator alloc, Deleter del, size_t maxSpares)
	: m_data(Spares(), maxSpares, std::move(alloc))
	, m_sharedData(std::make_shared<SharedData>(this, std::move(del)))
{
}

template <typename T, typename Alloc, typename Del>
inline Pool<T, Alloc, Del>::Pool(Pool&& other)
	: m_data(std::move(other.m_data))
{
	poolPtr() = this;
}

template <typename T, typename Alloc, typename Del>
inline Pool<T, Alloc, Del>::~Pool() {
	poolPtr() = nullptr;
}

template <typename T, typename Alloc, typename Del>
inline Pool<T, Alloc, Del>& Pool<T, Alloc, Del>::operator=(Pool&& other) {
	poolPtr() = nullptr;
	m_data = std::move(other.m_data);
	poolPtr() = this;
}



template <typename T, typename Alloc, typename Del>
void Pool<T, Alloc, Del>::setMaxSpares(size_t max) {
	maxSpares() = max;
}

template <typename T, typename Alloc, typename Del>
size_t Pool<T, Alloc, Del>::getMaxSpares() const {
	return maxSpares();
}



template <typename T, typename Alloc, typename Del>
inline typename Pool<T, Alloc, Del>::Ptr Pool<T, Alloc, Del>::acquire() {
	ElementType* el;

	if(spares().size() > 0) {
		el = spares().front().release();
		spares().pop_front();
	} else {
		el = allocator()();
	}

	assert(el);
	return Ptr(el, Recycler(*this));
}

template <typename T, typename Alloc, typename Del>
inline void Pool<T, Alloc, Del>::clear() {
	spares().clear();
}




template <typename T, typename Alloc, typename Del>
void Pool<T, Alloc, Del>::recycle(T* ptr) {
	if(spares().size() < maxSpares()) {
		//There is room for spares
		spares().emplace_back(ptr, deleter());
	} else {
		//No room for spares
		deleter()(ptr);
	}
}


template <typename T, typename Alloc, typename Del>
inline typename Pool<T, Alloc, Del>::SharedData& Pool<T, Alloc, Del>::sharedData() {
	assert(m_sharedData);
	return *m_sharedData;
}

template <typename T, typename Alloc, typename Del>
inline const typename Pool<T, Alloc, Del>::SharedData& Pool<T, Alloc, Del>::sharedData() const {
	assert(m_sharedData);
	return *m_sharedData;
}

template <typename T, typename Alloc, typename Del>
inline typename Pool<T, Alloc, Del>::Spares& Pool<T, Alloc, Del>::spares() {
	return std::get<m_spares>(m_data);
}

template <typename T, typename Alloc, typename Del>
inline const typename Pool<T, Alloc, Del>::Spares& Pool<T, Alloc, Del>::spares() const {
	return std::get<m_spares>(m_data);
}

template <typename T, typename Alloc, typename Del>
inline size_t& Pool<T, Alloc, Del>::maxSpares() {
	return std::get<m_maxSpares>(m_data);
}

template <typename T, typename Alloc, typename Del>
inline size_t Pool<T, Alloc, Del>::maxSpares() const {
	return std::get<m_maxSpares>(m_data);
}

template <typename T, typename Alloc, typename Del>
inline typename Pool<T, Alloc, Del>::Allocator& Pool<T, Alloc, Del>::allocator() {
	return std::get<m_allocator>(m_data);
}

template <typename T, typename Alloc, typename Del>
inline const typename Pool<T, Alloc, Del>::Allocator& Pool<T, Alloc, Del>::allocator() const {
	return std::get<m_allocator>(m_data);
}

template <typename T, typename Alloc, typename Del>
inline Pool<T, Alloc, Del>*& Pool<T, Alloc, Del>::poolPtr() {
	return std::get<m_poolPtr>(sharedData());
}

template <typename T, typename Alloc, typename Del>
inline Pool<T, Alloc, Del>* Pool<T, Alloc, Del>::poolPtr() const {
	return std::get<m_poolPtr>(sharedData());
}


template <typename T, typename Alloc, typename Del>
inline typename Pool<T, Alloc, Del>::Deleter& Pool<T, Alloc, Del>::deleter() {
	return std::get<m_deleter>(sharedData());
}

template <typename T, typename Alloc, typename Del>
inline const typename Pool<T, Alloc, Del>::Deleter& Pool<T, Alloc, Del>::deleter() const {
	return std::get<m_deleter>(sharedData());
}


/*
 * Pool::Recycler
 */

template <typename T, typename Alloc, typename Del>
Pool<T, Alloc, Del>::Recycler::Recycler(const Pool& pool)
	: m_sharedData(pool.m_sharedData)
{
}

template <typename T, typename Alloc, typename Del>
inline void Pool<T, Alloc, Del>::Recycler::operator()(T* ptr) const {
	if(poolPtr()) poolPtr()->recycle(ptr);
	else deleter()(ptr);
}

template <typename T, typename Alloc, typename Del>
inline const typename Pool<T, Alloc, Del>::SharedData& Pool<T, Alloc, Del>::Recycler::sharedData() const {
	assert(m_sharedData);
	return *m_sharedData;
}

template <typename T, typename Alloc, typename Del>
inline Pool<T, Alloc, Del>* Pool<T, Alloc, Del>::Recycler::poolPtr() const {
	return std::get<m_poolPtr>(sharedData());
}

template <typename T, typename Alloc, typename Del>
inline const typename Pool<T, Alloc, Del>::Deleter& Pool<T, Alloc, Del>::Recycler::deleter() const {
	return std::get<m_deleter>(sharedData());
}

}