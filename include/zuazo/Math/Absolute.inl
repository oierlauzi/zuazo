#include "Absolute.h"

#include "Rational.h"
#include "Vector.h"
#include "../Utils/CPU.h"

#include <cmath>

namespace Zuazo::Math {

template<typename T>
constexpr T abs(const T& x) {
	return (x < T()) ? -x : x;
}


constexpr int8_t abs(int8_t x) {
	const auto mask = x >> (sizeof(x)*Utils::getByteSize() - 1);
	return  (x + mask) ^ mask;
}

constexpr uint8_t abs(uint8_t x) {
	return x; //Unsigned
}

constexpr int16_t abs(int16_t x) {
	const auto mask = x >> (sizeof(x)*Utils::getByteSize() - 1);
	return  (x + mask) ^ mask;
}

constexpr uint16_t abs(uint16_t x) {
	return x; //Unsigned
}

constexpr int32_t abs(int32_t x) {
	const auto mask = x >> (sizeof(x)*Utils::getByteSize() - 1);
	return  (x + mask) ^ mask;
}

constexpr uint32_t abs(uint32_t x) {
	return x; //Unsigned
}

constexpr int64_t abs(int64_t x) {
	const auto mask = x >> (sizeof(x)*Utils::getByteSize() - 1);
	return  (x + mask) ^ mask;
}

constexpr uint64_t abs(uint64_t x) {
	return x; //Unsigned
}

inline float abs(float x) noexcept {
	return std::abs(x);	
}

inline double abs(double x) noexcept {
	return std::abs(x);	
}


template<typename num_t, typename den_t>
constexpr Rational<den_t, num_t> abs(const Rational<num_t, den_t>& r) noexcept {
	assert(r.getDenominator() >= 0); //Sign must be on the numerator
	return Rational<den_t, num_t>(abs(r.getNumerator()), r.getDenominator());
}

template<typename T, size_t N>
constexpr Vec<T, N> abs(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return abs(x);
		},
		v
	);
}


template<typename T>
constexpr T sign(const T& value) noexcept {
	if(value < T()) {
		return T(-1);
	} else if(value > T()) {
		return T(+1);
	} else {
		return T(0);
	}
}

template<typename T, size_t N>
constexpr Vec<T, N> sign(const Vec<T, N>& v) {
	return transform(
		[] (const auto& x) -> auto {
			return sign(x);
		},
		v
	);
}

}