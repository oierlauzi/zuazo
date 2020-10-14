#include "Pool.h"

#include "Functions.h"

#include <cassert>
#include <utility>

namespace Zuazo::Utils {

/*
 * Pool::Deleter
 */

template <typename T, typename Alloc>
class Pool<T, Alloc>::Deleter {
public:
	Deleter(Allocator& alloc) noexcept
		: m_allocator(alloc)
	{
	}

	Deleter(const Deleter& other) noexcept = default;
	~Deleter() = default;

	Deleter& operator=(const Deleter& other) noexcept = default;

	void operator()(ElementType* el) const {
		std::allocator_traits<Allocator>::destroy(m_allocator, el);
		std::allocator_traits<Allocator>::deallocate(m_allocator, el, 1);
	}

private:
	std::reference_wrapper<Allocator> m_allocator;
};


/*
 * Pool::Recycler
 */

template <typename T, typename Alloc>
class Pool<T, Alloc>::Recycler {
public:
	Recycler(std::shared_ptr<SharedData> sharedData) noexcept
		: m_sharedData(std::move(sharedData))
	{
	}

	Recycler(const Recycler& other) noexcept = default;
	Recycler(Recycler&& other) noexcept = default;
	~Recycler() = default;

	Recycler& operator=(const Recycler& other) noexcept = default;
	Recycler& operator=(Recycler&& other) noexcept = default;

	void operator()(ElementType* el) const {
		assert(m_sharedData);
		auto* poolPtr = std::get<poolPtrIdx>(*m_sharedData);

		if(poolPtr) {
			//Pool still exists
			poolPtr->recycle(el);
		} else {
			//Pool was deallocated. Delete the element
			Deleter del(std::get<allocatorIdx>(*m_sharedData));
			del(el);
		}
	}

private:
	std::shared_ptr<SharedData> m_sharedData;
};

/*
 * Pool
 */

template <typename T, typename Alloc>
inline Pool<T, Alloc>::Pool(size_t maxSpares, Allocator alloc)
	: m_sharedData(Utils::makeShared<SharedData>(this, std::move(alloc)))
	, m_maxSpareCount(maxSpares)
{
}

template <typename T, typename Alloc>
inline Pool<T, Alloc>::~Pool() {
	assert(m_sharedData);
	std::get<poolPtrIdx>(*m_sharedData) = nullptr; //So that they don't try to recycle
}



template <typename T, typename Alloc>
inline void Pool<T, Alloc>::setMaxSpareCount(size_t spares) noexcept {
	m_maxSpareCount = spares;
}

template <typename T, typename Alloc>
inline size_t Pool<T, Alloc>::getMaxSpareCount() const noexcept {
	return m_maxSpareCount;
}

template <typename T, typename Alloc>
inline size_t Pool<T, Alloc>::getSpareCount() const noexcept {
	return m_spares.size();
}



template <typename T, typename Alloc>
inline std::unique_ptr<typename Pool<T, Alloc>::ElementType, typename Pool<T, Alloc>::Recycler> 
Pool<T, Alloc>::acquire() {
	assert(m_sharedData);
	ElementType* result;

	if(m_spares.empty()) {
		Allocator& allocator = std::get<allocatorIdx>(*m_sharedData);

		//Allocate the a new element
		result = std::allocator_traits<Allocator>::allocate(allocator, 1);
		std::allocator_traits<Allocator>::construct(allocator, result);
	} else {
		//Acquire it from the spare queue
		result = m_spares.front().release();
		m_spares.pop();
	}

	assert(result);
	return std::unique_ptr<ElementType, Recycler>(
		result, 
		makeRecycler()
	);
}



template <typename T, typename Alloc>
inline typename Pool<T, Alloc>::Recycler Pool<T, Alloc>::makeRecycler() const noexcept {
	assert(m_sharedData);
	return Recycler(m_sharedData);
}

template <typename T, typename Alloc>
inline void Pool<T, Alloc>::recycle(ElementType* el) {
	assert(m_sharedData);
	std::unique_ptr<ElementType, Deleter> ptr(el, Deleter(std::get<allocatorIdx>(*m_sharedData)));

	if(m_spares.size() < m_maxSpareCount) {
		m_spares.emplace(std::move(ptr));
	}
}

}