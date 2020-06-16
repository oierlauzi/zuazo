#pragma once

#include <vector>
#include <memory>
#include <functional>

namespace Zuazo::Utils {

template <typename T>
class Pool {
public:
	using Ref = std::shared_ptr<T>;
	using AllocCallback = std::function<Ref()>;

	template<typename Func>
	explicit Pool(Func&& cbk);
	Pool(const Pool& other) = default;
	Pool(Pool&& other) = default;
	~Pool() = default;

	Pool& 						operator=(const Pool& other) = default;
	Pool& 						operator=(Pool&& other) = default;

	Ref							acquire() const;
	void						clear();
	void						clearUnused();

private:
	AllocCallback				m_allocCallback;
	mutable std::vector<Ref>	m_elements;

};

}

#include "Pool.inl"