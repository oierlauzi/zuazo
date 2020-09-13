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
	ClipBase(ClipBase&& other);
	virtual ~ClipBase();

	ClipBase& 						operator=(const ClipBase& other) = delete;
	ClipBase& 						operator=(ClipBase&& other);

	void							setState(State st);
	State							getState() const;

	void							setRepeat(Repeat repeat);
	Repeat							getRepeat() const;

	void							setTime(TimePoint tp);
	TimePoint						getTime() const;

	void							setPlaySpeed(double speed);
	double							getPlaySpeed() const;

	void							setProgress(double progress);
	double							getProgress() const;

	void							advance(Duration delta);
	void							rewind(Duration delta);
	void							advanceNormalSpeed(Duration delta);
	void							rewindNormalSpeed(Duration delta);
	void							nextStep();
	void							prevStep();
	void							play();
	void							pause();
	void							stop();
	bool							isPlaying() const;

	Duration						getDuration() const;
	Duration						getTimeStep() const;

protected:
	void							setRefreshCallback(RefreshCallback cbk);
	const RefreshCallback&			getRefreshCallback() const;

	void							setTimeStep(Duration step);
	void							setDuration(Duration dur);

private:
	struct Impl;
	Utils::Pimpl<Impl>				m_impl;

};

}