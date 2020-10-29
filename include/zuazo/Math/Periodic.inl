#include "Periodic.h"

#include "Absolute.h"

namespace Zuazo::Math {

template<typename T>
constexpr T sawtooth(const T& period, const T& value) noexcept {
    const auto positive = Math::mod(value, period) + period;
	return Math::mod(positive, period);
}

template<typename T, typename R>
constexpr R sawtooth(const T& period, const R& amplitude, const T& value) noexcept {
	return sawtooth(period, value) * amplitude / period;
}


template<typename T>
constexpr T triangle(const T& period, const T& value) noexcept {
	auto x = 2 * sawtooth(period, value);

	if(x > period) {
		return 2 * period - x; //Ramp down
	} else {
        return x; //Ramp up
    }
}

template<typename T, typename R>
constexpr R triangle(const T& period, const R& amplitude, const T& value) noexcept {
	return triangle(period, value) * amplitude / period;
}


template<typename T>
constexpr T pulse(const T& period, const T& uptime, const T& value) noexcept {
	if(sawtooth(value) < uptime) {
		return period;
	} else {
		return T();
	}
}

template<typename T, typename R>
constexpr R pulse(const T& period, const T& uptime, const R& amplitude, const T& value) noexcept {
	return pulse(period, uptime, value) * amplitude / period;
}

}