#pragma once

#include <vector>
#include <memory>
#include <tuple>
#include <utility>
#include <functional>

#include "Functions.h"

namespace Zuazo::Utils {

template <typename T, typename Alloc = decltype(&makeShared<T>)>
class Pool {
public:
	class Recycler;

	using ElementType = T;
	using Allocator = Alloc;

	explicit Pool(Allocator alloc = Allocator());
	Pool(const Pool& other) = delete;
	Pool(Pool&& other) = default;
	~Pool() = default;

	Pool& 						operator=(const Pool& other) = delete;
	Pool& 						operator=(Pool&& other) = default;

	std::shared_ptr<T>			acquire();
	void						clear();
	void						shrink(size_t maxSpares = 0);

private:
	using Spares = std::vector<std::shared_ptr<T>>;
	using Data = std::tuple<Spares, Allocator>; //In order to use EBO, store as a tuple

	enum DataIndices{
		m_spares,
		m_allocator
	};

	Data						m_data;

	Spares&						spares();
	const Spares&				spares() const;
	Allocator&					allocator();
	const Allocator&			allocator() const;

};

}

#include "Pool.inl"