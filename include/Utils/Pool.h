#pragma once

#include <forward_list>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <tuple>

namespace Zuazo::Utils {

template<typename T, typename... Args>
class Pool{
public:
	typedef std::tuple<Args...> Arguments;

	class Deleter{
	public:
		constexpr Deleter();
		constexpr Deleter(const Arguments& args, const Pool& owner);
		constexpr Deleter(const Deleter& other) = default;
		~Deleter() = default;

		void operator()(T* ptr) const;
	private:
		Arguments		m_args;
		const Pool*		m_owner;
	};

	friend Deleter;
	typedef std::unique_ptr<T, Deleter> unique_ptr;

	Pool() = default;
	Pool(const Pool& other) = delete;
	Pool(Pool&& other) = default;
	~Pool() = default;

	unique_ptr get(Args&&... args) const;
private:
	mutable std::mutex 	m_mutex;
	mutable std::unordered_map<Arguments, std::forward_list<std::unique_ptr<T>>> m_elements;
};

}

#include "Pool.inl"