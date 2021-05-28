#pragma once

#include "Macros.h"
#include "Chrono.h"
#include "Utils/Pimpl.h"

#include <utility>
#include <functional>

namespace Zuazo {

class ClipBase {
public:
	using RefreshCallback = std::function<void(ClipBase&)>;

	enum class State {
		paused,
		playing,
	};

	enum class Repeat {
		none = -1,
		repeat,
		pingPong
	};

	ClipBase(	Duration duration = {},
				Duration timeStep = {},
				RefreshCallback refreshCbk = {} );
	ClipBase(const ClipBase& other) = delete;
	ClipBase(ClipBase&& other) noexcept;
	virtual ~ClipBase();

	ClipBase& 						operator=(const ClipBase& other) = delete;
	ClipBase& 						operator=(ClipBase&& other) noexcept;

	void							setState(State st) noexcept;
	State							getState() const noexcept;

	void							setRepeat(Repeat repeat) noexcept;
	Repeat							getRepeat() const noexcept;

	void							setTime(TimePoint tp) noexcept;
	TimePoint						getTime() const noexcept;

	void							setPlaySpeed(double speed) noexcept;
	double							getPlaySpeed() const noexcept;

	void							setProgress(double progress) noexcept;
	double							getProgress() const noexcept;

	void							advance(Duration delta) noexcept;
	void							rewind(Duration delta) noexcept;
	void							advanceNormalSpeed(Duration delta) noexcept;
	void							rewindNormalSpeed(Duration delta) noexcept;
	void							nextStep() noexcept;
	void							prevStep() noexcept;
	void							play() noexcept;
	void							pause() noexcept;
	void							stop() noexcept;
	bool							isPlaying() const noexcept;

	Duration						getDuration() const noexcept;
	Duration						getTimeStep() const noexcept;

protected:
	void							setRefreshCallback(RefreshCallback cbk) noexcept;
	const RefreshCallback&			getRefreshCallback() const noexcept;

	void							setTimeStep(Duration step) noexcept;
	void							setDuration(Duration dur) noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>				m_impl;

};



ZUAZO_ENUM_ARITHMETIC_OPERATORS(ClipBase::State)
ZUAZO_ENUM_COMP_OPERATORS(ClipBase::State)	

std::string_view toString(ClipBase::State state) noexcept;
bool fromString(std::string_view str, ClipBase::State& state);
std::ostream& operator<<(std::ostream& os, ClipBase::State state);



ZUAZO_ENUM_ARITHMETIC_OPERATORS(ClipBase::Repeat)
ZUAZO_ENUM_COMP_OPERATORS(ClipBase::Repeat)	

std::string_view toString(ClipBase::Repeat repeat) noexcept;
bool fromString(std::string_view str, ClipBase::Repeat& repeat);
std::ostream& operator<<(std::ostream& os, ClipBase::Repeat repeat);


namespace Utils {

template<typename T>
class Any;

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<ClipBase::State> {
	static constexpr ClipBase::State first() noexcept { return ClipBase::State::paused; }
	static constexpr ClipBase::State last() noexcept { return ClipBase::State::playing; }
};

template<>
struct EnumTraits<ClipBase::Repeat> {
	static constexpr ClipBase::Repeat first() noexcept { return ClipBase::Repeat::repeat; }
	static constexpr ClipBase::Repeat last() noexcept { return ClipBase::Repeat::pingPong; }
};

}

}
