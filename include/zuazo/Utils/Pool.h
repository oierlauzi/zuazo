#pragma once

#include <deque>
#include <memory>
#include <tuple>
#include <utility>
#include <functional>

namespace Zuazo::Utils {

template <typename T, typename Alloc, typename Del = std::default_delete<T>>
class Pool {
public:
	class Recycler;

	using ElementType = T;
	using Allocator = Alloc;
	using Deleter = Del;
	using Ptr = std::unique_ptr<T, Recycler>;

	explicit Pool(	Allocator alloc = Allocator(), 
					Deleter del = Deleter(), 
					size_t maxSpares = 1 );
	Pool(const Pool& other) = delete;
	Pool(Pool&& other);
	~Pool();

	Pool& 						operator=(const Pool& other) = delete;
	Pool& 						operator=(Pool&& other);

	void						setMaxSpares(size_t max);
	size_t						getMaxSpares() const;

	Ptr							acquire();
	void						clear();

private:
	using Spares = std::deque<std::unique_ptr<ElementType, Deleter>>;
	using Data = std::tuple<Spares, size_t, Allocator>; //In order to use EBO, stored as a tuple
	using SharedData = std::tuple<Pool*, Deleter>; //In order to use EBO, stored as a tuple

	enum DataIndices{
		m_spares,
		m_maxSpares,
		m_allocator,
	};

	enum SharedDataIndices{
		m_poolPtr,
		m_deleter,
	};

	Data						m_data;
	std::shared_ptr<SharedData>	m_sharedData;

	void						recycle(T* ptr);

	SharedData&					sharedData();
	const SharedData&			sharedData() const;
	Spares&						spares();
	const Spares&				spares() const;
	size_t&						maxSpares();
	size_t						maxSpares() const;
	Allocator&					allocator();
	const Allocator&			allocator() const;
	Pool*&						poolPtr();
	Pool*						poolPtr() const;
	Deleter&					deleter();
	const Deleter&				deleter() const;

};

template <typename T, typename Alloc, typename Del>
class Pool<T, Alloc, Del>::Recycler {
	public:
		Recycler(const Pool& pool);
		Recycler(const Recycler& other) = default;
		Recycler(Recycler&& other) = default;
		~Recycler() = default;

		Recycler& 							operator=(const Recycler& other) = default;
		Recycler& 							operator=(Recycler&& other) = default;

		void 								operator()(T* ptr) const;

	private:
		std::shared_ptr<const SharedData>	m_sharedData;

		const SharedData&					sharedData() const;
		Pool*								poolPtr() const;
		const Deleter&						deleter() const;
	};

}

#include "Pool.inl"