#pragma once

#include "../Utils/Pimpl.h"

namespace Zuazo::Timing {

class Scheduler;

class MainLoop {
public:
	MainLoop(Scheduler& scheduler);
	MainLoop(const MainLoop& other) = delete;
	MainLoop(MainLoop&& other);
	~MainLoop();

	MainLoop&				operator=(const MainLoop& other) = delete;
	MainLoop&				operator=(MainLoop&& other);

	void					setScheduler(Scheduler& scheduler);
	Scheduler&				getScheduler() const;

private:
	struct Impl;
	Utils::Pimpl<Impl>		m_impl;

};

}