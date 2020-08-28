#include "Pimpl.h"

namespace Zuazo::Utils {

template<typename T, typename Alloc>
template<typename ...Args> 
inline Pimpl<T, Alloc>::Pimpl(Allocator alloc, Args&&... args)
	: m_data(
		AllocatorTraits::allocate(alloc, OBJECT_COUNT), 
		std::move(alloc)
	)
{
	AllocatorTraits::construct(
		getAllocator(),
		getPointer(),
		std::forward<Args>(args)...
	);
}

template<typename T, typename Alloc>
inline Pimpl<T, Alloc>::Pimpl(Pimpl&& other)
	: m_data(std::move(other.m_data))
{
	other.getPointer() = nullptr;
}

template<typename T, typename Alloc>
inline Pimpl<T, Alloc>::~Pimpl() {
	if(getPointer()) {
		AllocatorTraits::destroy(getAllocator(), getPointer());
		AllocatorTraits::deallocate(getAllocator(), getPointer(), OBJECT_COUNT);
	}
}

template<typename T, typename Alloc>
inline Pimpl<T, Alloc>& Pimpl<T, Alloc>::operator=(Pimpl&& other) {
	Pimpl(std::move(other)).swap(*this);
	return *this;
}



template<typename T, typename Alloc>
inline typename Pimpl<T, Alloc>::pointer Pimpl<T, Alloc>::operator->() {
	assert(get());
	return get();
}

template<typename T, typename Alloc>
inline typename Pimpl<T, Alloc>::const_pointer Pimpl<T, Alloc>::operator->() const {
	assert(get());
	return get();
}

template<typename T, typename Alloc>
inline typename Pimpl<T, Alloc>::reference Pimpl<T, Alloc>::operator*() {
	assert(get());
	return *(get());
}

template<typename T, typename Alloc>
inline typename Pimpl<T, Alloc>::const_reference Pimpl<T, Alloc>::operator*() const {
	assert(get());
	return *(get());
}

template<typename T, typename Alloc>
inline Pimpl<T, Alloc>::operator bool() const {
	return getPointer() != nullptr;
}



template<typename T, typename Alloc>
inline void Pimpl<T, Alloc>::swap(Pimpl& other) {
	std::swap(m_data, other.m_data);
}



template<typename T, typename Alloc>
inline typename Pimpl<T, Alloc>::pointer Pimpl<T, Alloc>::get() {
	return getPointer();
}

template<typename T, typename Alloc>
inline typename Pimpl<T, Alloc>::const_pointer Pimpl<T, Alloc>::get() const {
	return getPointer();
}



template<typename T, typename Alloc>
inline typename Pimpl<T, Alloc>::pointer& Pimpl<T, Alloc>::getPointer() {
	return std::get<m_pointer>(m_data);
}

template<typename T, typename Alloc>
inline const typename Pimpl<T, Alloc>::pointer& Pimpl<T, Alloc>::getPointer() const {
	return std::get<m_pointer>(m_data);
}

template<typename T, typename Alloc>
inline typename Pimpl<T, Alloc>::Allocator& Pimpl<T, Alloc>::getAllocator() {
	return std::get<m_allocator>(m_data);
}

template<typename T, typename Alloc>
inline const typename Pimpl<T, Alloc>::Allocator& Pimpl<T, Alloc>::getAllocator() const {
	return std::get<m_allocator>(m_data);
}

}
