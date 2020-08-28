#pragma once

#include <memory>
#include <type_traits>
#include <tuple>

namespace Zuazo::Utils {

template<typename T, typename Alloc = std::allocator<T>>
class Pimpl {
public:
	using Allocator = Alloc;
	using AllocatorTraits = std::allocator_traits<Allocator>;

	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	
    template<typename... Args> 
	explicit Pimpl(Allocator alloc, Args&&... args);
	Pimpl(const Pimpl& other) = delete;
	Pimpl(Pimpl&& other);
    ~Pimpl();

	Pimpl&				operator=(const Pimpl& other) = delete;
	Pimpl&				operator=(Pimpl&& other);

    pointer				operator->();
	const_pointer		operator->() const;
    reference			operator*();
	const_reference		operator*() const;

	operator bool() const;

	void				swap(Pimpl& other);

	pointer				get();
	const_pointer		get() const;

private:
	using Data = std::tuple<pointer, Allocator>;
	enum DataIndices {
		m_pointer,
		m_allocator
	};

	Data				m_data;

	pointer&			getPointer();
	const pointer&		getPointer() const;

	Allocator&			getAllocator();
	const Allocator&	getAllocator() const;

	static constexpr size_t OBJECT_COUNT = 1;
};

}

#include "Pimpl.inl"