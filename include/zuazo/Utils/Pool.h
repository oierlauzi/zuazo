#pragma once

#include <queue>
#include <memory>
#include <tuple>
#include <utility>
#include <functional>

namespace Zuazo::Utils {

template <typename T, typename Alloc = std::allocator<T>>
class Pool {
public:
	using ElementType = T;
	using Allocator = Alloc;
	class Recycler;

	explicit Pool(size_t maxSpareCount = 1, Allocator alloc = Allocator());
	Pool(const Pool& other) = delete;
	~Pool();

	Pool& 									operator=(const Pool& other) = delete;

	void									setMaxSpareCount(size_t spares) noexcept;
	size_t									getMaxSpareCount() const noexcept;
	size_t									getSpareCount() const noexcept;

	std::unique_ptr<ElementType, Recycler>	acquire();

private:
	class Deleter;

	//Using tuple so that EBO is used in case of sizeof(Allocator) = 0
	using SharedData = std::tuple<Pool*, Allocator>; 
	enum SharedDataIndices { poolPtrIdx, allocatorIdx };

	using Spares = std::queue<std::unique_ptr<ElementType, Deleter>>;

	std::shared_ptr<SharedData>				m_sharedData;
	Spares									m_spares;
	size_t									m_maxSpareCount;

	Recycler								makeRecycler() const noexcept;
	void									recycle(ElementType* el);

};

}

#include "Pool.inl"