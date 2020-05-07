#pragma once

#include <vector>
#include <variant>

namespace Zuazo::Utils {

template <typename T>
struct Limitation {
	struct Any {};
	struct Range { T min, max; };
	using Discrete = std::vector<T>;
	struct MustBe { T value; };
	struct None {};

	using Limit = std::variant<	Any,
								Range,
								Discrete,
								MustBe,
								None >;

	static constexpr bool isValid(const T& value, Any limit);
	static constexpr bool isValid(const T& value, const Range& limit);
	static constexpr bool isValid(const T& value, const Discrete& limit);
	static constexpr bool isValid(const T& value, const MustBe& limit);
	static constexpr bool isValid(const T& value, None limit);
	static constexpr bool isValid(const T& value, const Limit& limit);

	static constexpr Limit simplify(Any limit);
	static constexpr Limit simplify(const Range& limit);
	static constexpr Limit simplify(const Discrete& limit);
	static constexpr Limit simplify(const MustBe& limit);
	static constexpr Limit simplify(None limit);
	static constexpr Limit simplify(const Limit& limit);

	static constexpr Limit intersection(Any a, Any b);
	static constexpr Limit intersection(Any a, const Range& b);
	static constexpr Limit intersection(Any a, const Discrete& b);
	static constexpr Limit intersection(Any a, const MustBe& b);
	static constexpr Limit intersection(Any a, None b);
	static constexpr Limit intersection(const Range& a, Any b);
	static constexpr Limit intersection(const Range& a, const Range& b);
	static constexpr Limit intersection(const Range& a, const Discrete& b);
	static constexpr Limit intersection(const Range& a, const MustBe& b);
	static constexpr Limit intersection(const Range& a, None b);
	static constexpr Limit intersection(const Discrete& a, Any b);
	static constexpr Limit intersection(const Discrete& a, const Range& b);
	static constexpr Limit intersection(const Discrete& a, const Discrete& b);
	static constexpr Limit intersection(const Discrete& a, const MustBe& b);
	static constexpr Limit intersection(const Discrete& a, None b);
	static constexpr Limit intersection(const MustBe& a, Any b);
	static constexpr Limit intersection(const MustBe& a, const Range& b);
	static constexpr Limit intersection(const MustBe& a, const Discrete& b);
	static constexpr Limit intersection(const MustBe& a, const MustBe& b);
	static constexpr Limit intersection(const MustBe& a, None b);
	static constexpr Limit intersection(None a, Any b);
	static constexpr Limit intersection(None a, const Range& b);
	static constexpr Limit intersection(None a, const Discrete& b);
	static constexpr Limit intersection(None a, const MustBe& b);
	static constexpr Limit intersection(None a, None b);
	static constexpr Limit intersection(const Limit& a, const Limit& b);

	static constexpr T worst(Any limit);
	static constexpr T worst(const Range& limit);
	static constexpr T worst(const Discrete& limit);
	static constexpr T worst(const MustBe& limit);
	static constexpr T worst(None limit);
	static constexpr T worst(const Limit& limit);

	static constexpr T best(Any limit);
	static constexpr T best(const Range& limit);
	static constexpr T best(const Discrete& limit);
	static constexpr T best(const MustBe& limit);
	static constexpr T best(None limit);
	static constexpr T best(const Limit& limit);


};

}

#include "Limitation.inl"