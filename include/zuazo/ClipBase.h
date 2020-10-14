#pragma once

#include "Chrono.h"
#include "Utils/Pimpl.h"

#include <utility>
#include <functional>

namespace Zuazo {

class ClipBase {
public:
	using RefreshCallback = std::function<void(ClipBase&)>;

	enum class State {
		PAUSED,
		PLAYING,
	};

	enum class Repeat {
		NONE,
		REPEAT,
		PING_PONG
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

}