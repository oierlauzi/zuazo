#pragma once

#include <tuple>
#include <forward_list>
#include <memory>

/*
 * This code is based on:
 * https://hackernoon.com/the-object-pool-pattern-e424507b996a
 */

namespace Zuazo::Utils {

template<typename T, typename... Args>
class Pool{
	public:
		class Deleter{
		public:
			Deleter() = delete;
			constexpr Deleter(const Pool& owner);
			constexpr Deleter(const Deleter& other) = default;
			~Deleter() = default;

			void operator()(T* ptr) const;
		private:
			const Pool&		m_owner;
		};

		friend Deleter;

		typedef std::unique_ptr<T, Deleter> unique_ptr;

		Pool(Args&&... args);
		Pool(const Pool& other) = delete;
		Pool(Pool&& other) = default;
		~Pool() = default;

		unique_ptr										get() const;
	private:
		std::tuple<Args...>								m_arguments;
		mutable std::forward_list<std::unique_ptr<T>>	m_elements;

		T* constructElement(std::index_sequence<Is...> = std::index_sequence_for<Args...>()) const;
};

}

#include "Pool.inl"